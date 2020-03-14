#include "lcd_osc.h"
/*digits 0 - 7*/
uint16_t left_limit = 5;
const uint16_t font_3x5_d[] =
{
    1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1,
    1,1,1,1,1, 1,0,0,0,0, 0,0,0,0,0,
    1,1,1,0,1, 1,0,1,0,1, 1,0,1,1,1,
    1,1,1,1,1, 1,0,1,0,1, 1,0,1,0,1,
    0,1,1,1,1, 0,0,1,0,0, 1,1,1,0,0,
    1,0,1,1,1, 1,0,1,0,1, 1,1,1,0,1,
    1,0,1,1,1, 1,0,1,1,1, 1,1,1,1,1,
    1,1,1,1,1, 1,0,0,0,0, 1,0,0,0,0
};



volatile uint16_t start_page = 0;
volatile uint8_t sampled = 0;
volatile uint8_t lcd_osc_state = 0;
uint8_t * buffer_tr;
#define BUFFER_WIDTH (LCD_WIDTH * 2)
/*
*   Fill Screen with x and y axis
*/
void lcd_osc_init(int grid){
    lcd_init();
    uint16_t width = LCD_WIDTH, height = LCD_HEIGHT;
    /* Enable ports to switch modes*/
    DDRC &= ~((1<< PC2) | (1 << PC3) | (1 << PC4));
    PORTC &= ~((1<< PC2) | (1 << PC3)  | (1 << PC4));
    if(grid == 0){
        left_limit = 5;
        while(width--)
        {
            while(height--)
            {
                if(width == 2){
                    lcd_draw_pixel(LIME);
                }
                else if(height == 2){
                    lcd_draw_pixel(LIME);
                }
                else{
                    lcd_draw_pixel(BLACK);
                };
            }
            height = LCD_HEIGHT;
        }
    }
    /*
    * ..---.--|-  -  -  - 
    * 012345678
    */
    else {
        left_limit = 11;
        while(width--)
        {
            while(height--)
            {
                if(width == 9){
                    lcd_draw_pixel(LIME);
                }
                else if(height == 2){
                    lcd_draw_pixel(LIME);
                }
                else{
                    lcd_draw_pixel(BLACK);
                };
            }
            height = LCD_HEIGHT;
        }
        // draw digits for 0.37 (min) V 
        draw_digit_5x3(LCD_WIDTH - 8, LCD_HEIGHT - 9,7,WHITE);
        draw_digit_5x3(LCD_WIDTH - 4, LCD_HEIGHT - 9,3,WHITE);
        lcd_draw_pixel_at(LCD_WIDTH - 1,LCD_HEIGHT-5,LIME);
        // draw digits for 5,4,3,2,1 V
        int i = 0;
        #define digit_offset -2
        uint16_t y_values_digit[5] = {204 + digit_offset,153 + digit_offset,102 + digit_offset,51 + digit_offset,0};
        uint16_t y_values[5] = {204,153,102,51,0};
        for(i = 1; i < 6 ; i++)
        {
                draw_digit_5x3(LCD_WIDTH - 6, y_values_digit[i - 1],i,WHITE);
                lcd_draw_pixel_at(LCD_WIDTH - 8,y_values[i - 1],WHITE);
                lcd_draw_pixel_at(LCD_WIDTH - 9,y_values[i - 1],WHITE);
        }
    }
        
    

}

void osc_run()
{
    lcd_osc_init(1);
    lcd_osc_state = state_display_voltage;
    uint8_t shadow[LCD_WIDTH * 2] = {10};
    while(1)
    {
        if(lcd_osc_state == state_display_voltage)
        {
            osc_display_voltage(shadow);
        }
        else if(lcd_osc_state == state_display_screen)
        {
            osc_display_screen(shadow);
        }
        else if(lcd_osc_state == state_trigger_value)
        {
            osc_trigger_wave(shadow);
        }
    }

}


void osc_display_voltage(uint8_t * shadow){
    lcd_osc_clear_screen(shadow);
    uint8_t value;
    int x = LCD_WIDTH - left_limit;
    ADMUX = (1 << ADLAR) | (1 << REFS0) | 0b101; // enable internal reference value and set adc to ADC5
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(0<<ADPS1)|(0<<ADPS0); // enable adc and set prescalar

    while(lcd_osc_state == state_display_voltage){
        x = LCD_WIDTH - left_limit;
        while( x > 0)
        {
            value = 255 - readADC();
            if(value != shadow[x]){
                lcd_draw_pixel_at(x,shadow[x],BLACK);
                if(value > 236){
                    shadow[x] = 236;
                }
                else if( value < 0){
                    shadow[x] = 0;
                }
                else{
                    shadow[x] = value;
                }
                lcd_draw_pixel_at(x,shadow[x],BLUE);
            }
            x--;
        };
        if((PINC & (1 << PC3)))
        {
            lcd_osc_state = state_trigger_value;
        }
        else if((PINC & (1 << PC4)))
        {
            lcd_osc_state = state_display_screen;
        }
    }
}


void osc_display_screen(uint8_t * shadow)
{
    //lcd_osc_clear_screen();
    uint8_t buffer[LCD_WIDTH];
    while(lcd_osc_state == state_display_screen)
    {
        sample_data(buffer,LCD_WIDTH - left_limit);
        draw_screen_cont(buffer,shadow);
        //adc_prescaler = 0b111 - (readInpPre() >> 5);
        if((PINC & (1 << PC3)))
        {
            lcd_osc_state = state_display_voltage;
        }
        else if((PINC & (1 << PC4)))
        {
            lcd_osc_state = state_trigger_value;
        }
    }
}

void osc_trigger_wave(uint8_t * shadow)
{
    //lcd_osc_clear_screen();
    //adc_prescaler = 0b111 - (readInpPre() >> 5);
    sei();
    uint8_t buffer_trigger[ BUFFER_WIDTH ];
    buffer_tr = buffer_trigger;
    start_page = 0;
    sampled = 0;
    
    ACSR = (0 << ACD) | (0 << ACBG) | (1 << ACI) | ( 1 << ACIE) | (0b11 <<ACIS0) ;
    DIDR1 |= (0b11 << AIN0D);
    while(sampled == 0){};
    sampled = 0;
    int trigger_allowed = 0;
    ACSR &= ~(1 << ACIE);
    while(lcd_osc_state == state_trigger_value)
    {
        cli();
        draw_screen_cont(&buffer_trigger[start_page],shadow);
        ACSR |= (1 << ACI);
        sei();
        while(1)
        {
            if(trigger_allowed == 0)
            {
                if(PINC & (1 << PC2))
                {
                    ACSR |= (1 << ACIE) | (1<< ACI);
                    trigger_allowed =1;
                }
            }
            else
            {
                if((PINC & (1 << PC2)) == 0)
                {
                    ACSR &= ~(1 << ACIE);
                    trigger_allowed = 0;
                }
            }
            
            
            if(sampled == 1){
                start_page = 0;
                sampled = 0;
                if((PINC & (0b11 << PC3)) == (0b11 << PC3))
                {
                    lcd_osc_state = state_display_voltage;
                }
                break;
            }
            else if((PINC & (1 << PC3)))
            {
                if(PINC & (1 << PC4))
                {
                    lcd_osc_state = state_display_voltage;
                    break;
                }
                start_page += 10;
                if(start_page >= (BUFFER_WIDTH - (LCD_WIDTH - left_limit)))
                {
                    start_page =  (BUFFER_WIDTH - (LCD_WIDTH - left_limit + 1));
                }
                break;
            }
            else if((PINC & (1 << 4)))
            {
                if(PINC & (1 << PC3))
                {
                    lcd_osc_state = state_display_voltage;
                    break;
                }
                if(start_page < 11 )
                {
                    start_page = 0;
                }
                else
                {
                    start_page-= 10;
                }
                break;
            };
        };
    }
    cli();
}


void draw_digit_5x3(uint16_t x, uint16_t y ,uint8_t nr, uint16_t colour)
{
    int n,j;
    for(n = 0; n < 3 ; n++)
    {
        lcd_set_cursor(x + n,y);
        for(j = 0; j < 5; j++)
        {
            lcd_draw_pixel(colour*font_3x5_d[(15*nr) + (n * 5) + j ]);
        }
    }
}



void sample_data(uint8_t *  buffer, uint32_t size )
{

    ADMUX = (1 << ADLAR) | (1 << REFS0) | 0b101; // enable internal reference value and set adc to ADC5
    ADCSRA = (1<<ADEN)| (( 0b011) << ADPS0); // enable adc and set prescalar
    int i = 0;
    ADCSRA |= (1<<ADSC);
    for(i = 0; i < size; i++)
    {
        buffer[i] = 255 - readADC_sample();
    }
    ACSR &= ~(1 << ACI);
    while(ADCSRA & (1<<ADSC));
}
void draw_screen(uint8_t * buffer,uint8_t * shadow  )
{
    uint8_t * v_ptr = buffer;
    int x = LCD_WIDTH - left_limit;
    while(x > 0)
    {
        lcd_draw_pixel_at(x,shadow[x],BLACK);
        if(*v_ptr > 236){
            shadow[x] = 236;
            }
        else if( *v_ptr < 0){
            shadow[x] = 0;
        }
        else{
            shadow[x] = *v_ptr;
        }
        lcd_draw_pixel_at(x,shadow[x],BLUE);
        x--;
        v_ptr++;
    }
}
/*
 * Shadow has to be 2 * lcd size
 */
void draw_screen_cont(uint8_t * buffer, uint8_t * shadow)
{
    uint8_t * v_ptr = buffer;
    uint8_t prev_val;
    int x = LCD_WIDTH - left_limit;
    int s = x *2;
    lcd_draw_pixel_at(x,shadow[s],BLACK);
    if(*v_ptr > 236){
        shadow[x] = 236;
        }
    else if( *v_ptr < 0){
        shadow[x] = 0;
    }
    else{
       shadow[x] = *v_ptr;
    }
    lcd_draw_pixel_at(x,shadow[x],BLUE);
    prev_val = shadow[x];
    x--;
    v_ptr++;
    s--;
    uint8_t value = 0;
    //uint8_t stride =readInpPre() >> 6;
    while(x > 0)
    {

        lcd_draw_pixel_at(x,shadow[x],BLACK);
        for(int i = 1; i <= shadow[s]; i++){
            if((shadow[x] + i )> 236)
            {
                break;
            }
            lcd_draw_pixel(BLACK);
        }
        if(*v_ptr >236)
        {
            value = 236;
        }
        else if( *v_ptr < 0){
            value = 0;
        }
        else{
            value = *v_ptr;
        }
        if((abs(value - prev_val)) < 2)
        {
            shadow[x] = value;
            shadow[s] = 0;
            lcd_draw_pixel_at(x,shadow[x],BLUE);
        }
        else if(value > prev_val)
        {
            shadow[x] = prev_val;
            int t0 = 0;
            lcd_draw_pixel_at(x,shadow[x],BLUE);
            for(int k = shadow[x] ; k < value; k++)
            {
                lcd_draw_pixel(BLUE);
                t0++;
            }
            shadow[s] = t0;
        }
        else
        {
            shadow[x] = value;
            int t0 = 0;
            lcd_draw_pixel_at(x,value,BLUE);
            for(int k = value; k < prev_val; k++){
                lcd_draw_pixel(BLUE);
                t0++;
            }
            shadow[s] = t0;
        }
        s--;
        x--;
        v_ptr++;
        prev_val = value;
        
    }

}

void draw_screen_cont_ns(uint8_t * buffer)
{
    uint8_t * v_ptr = buffer;
    uint8_t prev_val;
    uint8_t value;
    int x = LCD_WIDTH - left_limit;
    while( x > 0)
    {
        if(*v_ptr > 236){
            value = 236;
            }
        else if( *v_ptr < 0){
            value= 0;
        }
        else{
        value = *v_ptr;
        }
        if(x == (LCD_WIDTH - left_limit))
        {
            prev_val = value;
        }
        /* NO shadow*/
        int i = 0;
        if((abs(value - prev_val)) < 2)
        {
            lcd_set_cursor(x,0);
            while(i < value)
            {
                lcd_draw_pixel(BLACK);
                i++;
            }
            lcd_draw_pixel(BLUE);
            i++;
            while(i <= 236)
            {
                lcd_draw_pixel(BLACK);
                i++;
            }

        }
        else if(value > prev_val)
        {
            lcd_set_cursor(x,0);
            while(i < prev_val)
            {
                lcd_draw_pixel(BLACK);
                i++;
            }
            while(i <= value)
            {
                lcd_draw_pixel(BLUE);
                i++;
            }
            while(i <= 236)
            {
                lcd_draw_pixel(BLACK);
                i++;
            }
        }
        else
        {
            lcd_set_cursor(x,0);
            while(i < value)
            {
                lcd_draw_pixel(BLACK);
                i++;
            }
            while(i <= prev_val)
            {
                lcd_draw_pixel(BLUE);
                i++;
            }
            while(i <= 236)
            {
                lcd_draw_pixel(BLACK);
                i++;
            }
        }
        x--;
        v_ptr++;
        prev_val = value;
        
    }

}


void trigger_wave(uint8_t * shadow)
{
    uint8_t buffer_trigger[ BUFFER_WIDTH ];
    buffer_tr = buffer_trigger;
    start_page = 0;
    sampled = 0;
    EICRA |= 0b11 << ISC00;
    EIMSK |= 1 << INT0;
    DDRC &= ~((1 << PC3) | (1 << PC4));
    PORTC &= ~((1 << PC3)  | (1 << PC4));
    
    sample_data(buffer_trigger, BUFFER_WIDTH);
    ACSR = (0 << ACD) | (0 << ACBG) | ( 1 << ACIE) | (0b11 <<ACIS0) ;
    DIDR1 |= (0b11 << AIN0D);
    sei();
    while(1)
    {
        draw_screen_cont(&buffer_trigger[start_page],shadow);
        while(1)
        {
            if(sampled == 1){
                start_page = 0;
                sampled = 0;
                break;
            }
            else if((PINC & (1 << PC3)))
            {
                start_page += 10;
                if(start_page >= (BUFFER_WIDTH - (LCD_WIDTH - left_limit)))
                {
                    start_page =  (BUFFER_WIDTH - (LCD_WIDTH - left_limit + 1));
                }
                break;
            }
            else if((PINC & (1 << 4)))
            {
                if(start_page < 11 )
                {
                    start_page = 0;
                }
                else
                {
                    start_page-= 10;
                }
                break;
            };
        };
    }


    
}


void lcd_osc_clear_screen(uint8_t * shadow)
{
    
    int x = LCD_WIDTH - left_limit;

    int s = x*2;
    while( x > 0){
        lcd_draw_pixel_at(x,shadow[x],BLACK);
        for(int i = 1; i <= shadow[s]; i++){
            if((shadow[x] + i )> 236)
            {
                break;
            }
            lcd_draw_pixel(BLACK);
        }
        x--;
        s--;
    }

}

ISR(ANALOG_COMP_vect)
{
    sampled = 1;
    sample_data(buffer_tr,BUFFER_WIDTH);
}

uint8_t readADC()
{
  ADCSRA |= (1<<ADSC);
  while(ADCSRA & (1<<ADSC));
  return (ADCH );
}

uint8_t readADC_sample()
{
  while(ADCSRA & (1<<ADSC));
  ADCSRA |= (1<<ADSC);
  return (ADCH );
}