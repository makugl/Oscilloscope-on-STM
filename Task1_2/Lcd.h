#ifndef LCD_H
#define LCD_H


#include <stdint.h>


typedef struct RGB565_s {
    unsigned int B:5;
    unsigned int G:6;
    unsigned int R:5;
} RGB565_t;
  
extern const RGB565_t Red;
extern const RGB565_t Green;
extern const RGB565_t Blue;
extern const RGB565_t White;
extern const RGB565_t Black;
extern const RGB565_t Gray;

int Lcd_Initialize(void);
void Lcd_Freeze(void);
void Lcd_Unfreeze(void);
void Lcd_FillRectangle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t width, uint_fast16_t height, RGB565_t color);
void Lcd_Draw(uint_fast16_t x, uint_fast16_t y, uint_fast16_t width, uint_fast16_t height, RGB565_t const * data);


int Lcd_WriteCharacter(uint_fast16_t * px, uint_fast16_t y, char character, RGB565_t foreground, RGB565_t background);
int Lcd_WriteString(uint_fast16_t x, uint_fast16_t y, const char * string, RGB565_t foreground, RGB565_t background);


#endif // LCD_H
