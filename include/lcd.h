/*******************************************************************************
 * File:        lcd.h
 * Project:     ILI9341
 * Author:      Nicolas Meyert�ns
 * Version:     siehe main.h
 * Web:         http://pic-projekte.de
 ******************************************************************************/

#ifndef LCD_H
#define	LCD_H

//#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#include <util/delay.h>
/*******************************************************************************
 * Pin assignment
 * Port B
 */
#define LCD_DC  6    /* Data or Command */
#define LCD_CS  7    /* SPI Chip select */  
#define LCD_RST 0


#define DD_MISO PB4
#define DD_MOSI PB3
#define DD_SCK PB5
#define DD_SS PB2 
/*******************************************************************************
 * Usefull Makros for the LCD-Functions
 */

#define LCD_REG     0
#define LCD_DATA    1

#define LCD_WIDTH   320
#define LCD_HEIGHT  240	

#define WHITE       0xFFFF
#define SILVER      0xC618
#define BLACK       0x0000
#define GREY        0x8410
#define RED         0xF800
#define MAROON      0x8000
#define FUCHSIA     0xF81F		
#define PURPLE      0x8010
#define LIME        0x07E0
#define GREEN       0x0400
#define YELLOW      0xFFE0
#define OLIVE       0x8400
#define BLUE        0x001F
#define NAVY        0x0010
#define AQUA        0x07FF
#define TEAL        0x0410
#define DARK_BLUE   0x0002

/*******************************************************************************
 * Bekanntmachung globaler Variablen
 */

extern uint16_t global_bg_color;

/*******************************************************************************
 * Function prototypes with JavaDOC
 */

/**
 * This function will initialize the controller of the LC-Display
 */
void lcd_init (void);

/**
 * This function send data or register-information via SPI to the LC-Display
 * @param dc 0: Register-Information 1: Data-Information
 * @param value Value to send to the LCD-Controller
 */
void lcd_send(bool dc, uint8_t value);

/**
 * Set the cursor at position (x,y)
 * @param x X-Position [0..320]
 * @param y Y-Position [0..240]
 * @return EXIT_SUCCESS / EXIT_FAILURE
 */
uint8_t lcd_set_cursor(uint16_t x, uint16_t y);

/**
 * Set the cursor to the coordinate (x,_)
 * @param x X-Coordinate
 * @return EXIT_SUCCESS / EXIT_FAILURE
 */
uint8_t lcd_set_cursor_x(uint16_t x);

/**
 * Set the cursor to the coordinate (_,y)
 * @param y Y-Coordinate
 * @return EXIT_SUCCESS / EXIT_FAILURE
 */
uint8_t lcd_set_cursor_y(uint16_t y);

/**
 * Draw a Pixel at the actual position
 * @param color The foreground color of the pixel
 * @return EXIT_SUCCESS / EXIT_FAILURE
 */
uint8_t lcd_draw_pixel(uint16_t color);


/**
 * This function will draw a Pixel at (x,y)
 * @param x X-Coordinate
 * @param y Y-Coordinate
 * @param color Foreground color of the Pixel
 */
void lcd_draw_pixel_at(uint16_t x, uint16_t y, uint16_t color);

#endif	/* LCD_H */
