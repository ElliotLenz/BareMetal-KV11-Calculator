#ifndef LCD_HD44780_H
#define LCD_HD44780_H
#include <stdint.h>

void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_WriteChar(char c);
void LCD_WriteString(const char* str);
void LCD_Init(void);

#endif