#ifndef LCD_OSC_H
#define LCD_OSC_H 100
#include "lcd.h"

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define state_display_voltage 0 
#define state_display_screen 1
#define state_trigger_value 2


extern uint16_t left_limit;

void lcd_osc_init();

/*
 * Runs the Oscilloscope
 */

void osc_run();
/*
 * Displays the input voltage on the screen
 * Interuppt by using a ISR?
*/
void osc_display_voltage(uint8_t * shadow);

/*
 * Samples one screen worth of data,
 * displays it and samples new data 
 * directly afterwards
 */
void osc_display_screen(uint8_t * shadow);

/*
 * Samples a certain amount of data and keeps it
 * stable until another triggering
 */
void osc_trigger_wave(uint8_t * shadow);




/*
 * Clears the screen based on the shadow register provided
 */

void lcd_osc_clear_screen(uint8_t * shadow);

/*
 * Stores a certain amount of data into a buffer
 * 
 */
void sample_data(uint8_t *  buffer, uint32_t size );


/*
 * Draws a simple 5x3 digit (0-9) at position x,y 
 */
void draw_digit_5x3(uint16_t x, uint16_t y ,uint8_t nr, uint16_t colour);


/*
 * reads the ADC, the control register has to be set before calling
 * this function
 */
uint8_t readADC();

/*
 * reads the ADC data register first than starts a new conversion
 */
uint8_t readADC_sample();



/* 
 * Displays buffer to screen, deletes previous values if shadow exists
 * The shadow buffer has to be allocated with size (LCD_WIDTH - left_limit - 1)
 */ 

void draw_screen_cont(uint8_t * buffer, uint8_t * shadow);

/*
 * Displays buffer to screen, clears all the rows as shadow register
 * is not used.
 * Slower than draw_screen_cont but does not need as much memory
 */

void draw_screen_cont_ns(uint8_t * buffer);
#endif