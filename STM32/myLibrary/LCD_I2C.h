#ifndef LCD_I2C_H
#define LCD_I2C_H

void LCD_Init (void);
void LCD_Gotoxy(uint8_t x, uint8_t y);
void LCD_Printf(const char* str, ...);
void LCD_Clear(void);

#endif
