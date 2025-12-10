#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Core LCD control functions
void LCD_enablePulse();
void LCD_send4Bits(uint8_t data);
void LCD_command(uint8_t cmd);
void LCD_data(uint8_t data);

// New string functions
void LCD_printString(char *str);
void LCD_writeOrMenu(char *data);

// LCD Initialization
void LCD_init();

#endif
