/*******************************************************************************
 * File:        lcd.c
 * Project:     ILI9341
 * Author:      Nicolas Meyert�ns
 * Version:     siehe main.h
 * Web:         http://pic-projekte.de
 ******************************************************************************/

#include <stdlib.h>
#include "lcd.h"
//#include "font.h"
#include <math.h>
#include <avr/pgmspace.h>
/*******************************************************************************
 * Globale Variablen
 */

uint16_t global_bg_color = RED;



void msDelay (uint8_t ms)
{
        uint8_t k;
        
        for(k=0; k<ms; k++)
        {
            _delay_ms(1);
        }
}

/*******************************************************************************
 * Initialisierung des Displays mit einer frei waehlbaren Hintergrundfarbe
 */
  #define scl_sw 0
  #define sda_sw 1
void lcd_init (void)
{    
    DDRD |= (1 << sda_sw) | (1 << scl_sw);
    DDRB = (1 << LCD_DC)  | (1 << LCD_RST) | (1 << LCD_CS) | (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
    PORTB |= (1 << DD_SS);
    SPCR = (1<<SPE)|(1<<MSTR)|(0b00<<SPR0);
    SPSR |= (1 << SPI2X);
    int i = 0;
    uint16_t val;
    PORTB &= ~(1 << LCD_RST);
    _delay_ms(100);
    PORTB |= (1 << LCD_RST);
    lcd_send(LCD_REG,  0xCB);
    lcd_send(LCD_DATA, 0x39);
    lcd_send(LCD_DATA, 0x2C);
    lcd_send(LCD_DATA, 0x00);
    lcd_send(LCD_DATA, 0x34);
    lcd_send(LCD_DATA, 0x02);

    lcd_send(LCD_REG,  0xCF);
    lcd_send(LCD_DATA, 0x00);
    lcd_send(LCD_DATA, 0XC1);
    lcd_send(LCD_DATA, 0X30);

    lcd_send(LCD_REG,  0xE8);
    lcd_send(LCD_DATA, 0x85);
    lcd_send(LCD_DATA, 0x00);
    lcd_send(LCD_DATA, 0x78);

    lcd_send(LCD_REG,  0xEA);
    lcd_send(LCD_DATA, 0x00);
    lcd_send(LCD_DATA, 0x00);

    lcd_send(LCD_REG,  0xED);
    lcd_send(LCD_DATA, 0x64);
    lcd_send(LCD_DATA, 0x03);
    lcd_send(LCD_DATA, 0X12);
    lcd_send(LCD_DATA, 0X81);

    lcd_send(LCD_REG,  0xF7);
    lcd_send(LCD_DATA, 0x20);

    lcd_send(LCD_REG,  0xC0); // Power control
    lcd_send(LCD_DATA, 0x23); // VRH[5:0]

    lcd_send(LCD_REG,  0xC1); // Power control
    lcd_send(LCD_DATA, 0x10); // SAP[2:0];BT[3:0]

    lcd_send(LCD_REG,  0xC5); // VCM control
    lcd_send(LCD_DATA, 0x3e);
    lcd_send(LCD_DATA, 0x28);

    lcd_send(LCD_REG,  0xC7); // VCM control2
    lcd_send(LCD_DATA, 0x86);

    lcd_send(LCD_REG,  0x36); // Memory Access Control
    lcd_send(LCD_DATA, 0x88); // C8

    lcd_send(LCD_REG,  0x3A);
    lcd_send(LCD_DATA, 0x55);

    lcd_send(LCD_REG,  0xB1);
    lcd_send(LCD_DATA, 0x00);
    lcd_send(LCD_DATA, 0x18);

    lcd_send(LCD_REG,  0xB6); // Display Function Control
    lcd_send(LCD_DATA, 0x08);
    lcd_send(LCD_DATA, 0x82);
    lcd_send(LCD_DATA, 0x27);

    lcd_send(LCD_REG,  0xF2); // 3Gamma Function Disable
    lcd_send(LCD_DATA, 0x00);

    lcd_send(LCD_REG,  0x26); // Gamma curve selected
    lcd_send(LCD_DATA, 0x01);

    lcd_send(LCD_REG,  0xE0); // Set Gamma
    lcd_send(LCD_DATA, 0x0F);
    lcd_send(LCD_DATA, 0x31);
    lcd_send(LCD_DATA, 0x2B);
    lcd_send(LCD_DATA, 0x0C);
    lcd_send(LCD_DATA, 0x0E);
    lcd_send(LCD_DATA, 0x08);
    lcd_send(LCD_DATA, 0x4E);
    lcd_send(LCD_DATA, 0xF1);
    lcd_send(LCD_DATA, 0x37);
    lcd_send(LCD_DATA, 0x07);
    lcd_send(LCD_DATA, 0x10);
    lcd_send(LCD_DATA, 0x03);
    lcd_send(LCD_DATA, 0x0E);
    lcd_send(LCD_DATA, 0x09);
    lcd_send(LCD_DATA, 0x00);

    lcd_send(LCD_REG,  0xE1); // Set Gamma
    lcd_send(LCD_DATA, 0x00);
    lcd_send(LCD_DATA, 0x0E);
    lcd_send(LCD_DATA, 0x14);
    lcd_send(LCD_DATA, 0x03);
    lcd_send(LCD_DATA, 0x11);
    lcd_send(LCD_DATA, 0x07);
    lcd_send(LCD_DATA, 0x31);
    lcd_send(LCD_DATA, 0xC1);
    lcd_send(LCD_DATA, 0x48);
    lcd_send(LCD_DATA, 0x08);
    lcd_send(LCD_DATA, 0x0F);
    lcd_send(LCD_DATA, 0x0C);
    lcd_send(LCD_DATA, 0x31);
    lcd_send(LCD_DATA, 0x36);
    lcd_send(LCD_DATA, 0x0F);

    lcd_send(LCD_REG,  0x11); // Sleep out
    msDelay(120);
    lcd_send(LCD_REG, 0x2c);  
    //lcd_fill(BLACK);

    lcd_send(LCD_REG, 0x29); // Display on 
    lcd_send(LCD_REG, 0x2c);
    
}

/*******************************************************************************
 * Daten werden via SPI-Bus an den Display-Controller gesendet
 */

void lcd_send(bool dc, uint8_t value)
{   
    /* register or data */
    if(dc){
        PORTB  |= (1 << LCD_DC);
    }
    else{
        PORTB  &= ~(1 << LCD_DC);

    }
    //LCD_PORT  &= (dc << LCD_DC) | ~(1 < LCD_DC);

    /* transmit via SPI */
    PORTB &= ~(1 << LCD_CS);
    
  
  SPDR = value;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF))){};
  PORTB |= (1 << LCD_CS);
}

/*******************************************************************************
 * Platzieren des Cursors an Punkt (x,y)
 */

uint8_t lcd_set_cursor(uint16_t x, uint16_t y)
{
    if( lcd_set_cursor_x(x) || lcd_set_cursor_y(y) )
    {
        return EXIT_FAILURE;
    }
	
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Teilplatzierung des Cursors an (x,_)
 */

uint8_t lcd_set_cursor_x(uint16_t x)
{
    if( x >= LCD_WIDTH )
    {
        return EXIT_FAILURE;
    }
    
    lcd_send(LCD_REG,  0x2B);
    lcd_send(LCD_DATA, x >> 8);
    lcd_send(LCD_DATA, x & 0xFF);
    lcd_send(LCD_REG, 0x2c);
    
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Teilplatzierung des Cursors an (_,y)
 */

uint8_t lcd_set_cursor_y(uint16_t y)
{
    if( y >= LCD_HEIGHT ) 
    {
        return EXIT_FAILURE;
    }
    
    lcd_send(LCD_REG,  0x2A);
    lcd_send(LCD_DATA, y >> 8);
    lcd_send(LCD_DATA, y & 0xFF);
    lcd_send(LCD_REG, 0x2c);
    
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Zeichnen eines Pixels an die aktuelle Cursorposition
 */

uint8_t lcd_draw_pixel(uint16_t color)
{
    lcd_send(LCD_DATA, color >> 8);
    lcd_send(LCD_DATA, color & 0xFF);
    
    return EXIT_SUCCESS;
}


/*******************************************************************************
 * Zeichnen eines Pixels an den Punkt (x,y) mit Farbe <color>
 */

void lcd_draw_pixel_at(uint16_t x, uint16_t y, uint16_t color)
{
    if( !lcd_set_cursor(x,y) )
    {
        lcd_draw_pixel(color);
    }
}
