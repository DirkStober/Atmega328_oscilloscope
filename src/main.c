/*
ATmega8, 48, 88, 168, 328

    /Reset PC6|1   28|PC5       SCL
Nokia.SCL  PD0|2   27|PC4       SDA
Nokia.SDIN PD1|3   26|PC3     
Nokia.DC   PD2|4   25|PC2       LCD_RS ( 3) 
Nokia.SCE  PD3|5   24|PC1       LCD_E  ( 5) 
Nokia.RST  PD4|6   23|PC0       LCD_RW ( 6) 
           Vcc|7   22|Gnd                   
           Gnd|8   21|Aref                  
Xtal       PB6|9   20|AVcc                  
Xtal       PB7|10  19|PB5 SCK   LCD_D5 (11) 
           PD5|11  18|PB4 MISO  LCD_D4 (12) 
           PD6|12  17|PB3 MOSI  LCD_D7 (13) 
           PD7|13  16|PB2       LCD_D6 (14) 
           PB0|14  15|PB1       switch (17) 
*/

#define F_CPU 1000000UL

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#include "lcd_osc.h"

  
int main(void)
{
  osc_run();
}



