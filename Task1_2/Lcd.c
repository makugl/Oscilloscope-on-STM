// Include module header files.
#include "Lcd.h"

// Include custom header files.
#include "Platform.h"
#include "Gpio.h"

// Include toolchain header files.
#include <stm32f072xb.h>

// Include C standard library header files.
#include <stdbool.h>
#include <stddef.h>



#define GPIO      (GPIOE)
#define GPIO_CS   (((uint32_t)1) << 8)
#define GPIO_DC   (((uint32_t)1) << 9)
#define GPIO_WR   (((uint32_t)1) << 10)
#define GPIO_RD   (((uint32_t)1) << 11)
#define GPIO_DATA (((uint32_t)0xFF))

static inline void Lcd_Write(uint8_t data, bool command)
{
  // Timing:
  //  48 MHz -> 1 Cycle = 20.833 ns
  
  // WR: low (active), RD: high (inactive)
  GPIO->BSRR = ((GPIO_WR) << 16) | (GPIO_RD);

  // Set D/C and Data
  GPIO->BSRR = (((GPIO_DC)   << 16) | (command ? 0 : GPIO_DC))
             | (((GPIO_DATA) << 16) | data);
  
  // WR: high (inactive), RD: high (inactive)
  GPIO->BSRR = (GPIO_WR | GPIO_RD);
}

static inline void Lcd_WritePixel(RGB565_t value)
{
  Lcd_Write((*((uint16_t *)&value)) >> 8, false);
  Lcd_Write( *((uint16_t *)&value),       false);
}

static void Lcd_SetPosition(uint_fast16_t xStart, uint_fast16_t xEnd, uint_fast16_t yStart, uint_fast16_t yEnd)
{
  if(xStart > 239)
  {
    xStart = 239;
  }
  if(xEnd > 239)
  {
    xEnd = 239;
  }
  if(yStart > 319)
  {
    yStart = 319;
  }
  if(yEnd > 319)
  {
    yEnd = 319;
  }
  
  Lcd_Write(0x2A, true);
  Lcd_Write((xStart >> 8) & 0xFF, false);
  Lcd_Write( xStart       & 0xFF, false);
  Lcd_Write((xEnd >> 8)   & 0xFF, false);
  Lcd_Write( xEnd         & 0xFF, false);
  
  Lcd_Write(0x2B, true);
  Lcd_Write((yStart >> 8) & 0xFF, false);
  Lcd_Write( yStart       & 0xFF, false);
  Lcd_Write((yEnd >> 8)   & 0xFF, false);
  Lcd_Write( yEnd         & 0xFF, false);
}

static bool Lcd_Mode = true;

static inline int Lcd_SetMode(bool write)
{
  if(Lcd_Mode == write)
  {
    return ERROR_NONE;
  }
  
  Lcd_Mode = write;
  
  Gpio_Mode_t mode = write
    ? GPIO_MODE_OUTPUT
    : GPIO_MODE_INPUT;

  int error;
  
  error = Gpio_Initialize(GPIO_ID_TFT_D0, mode, GPIOE->ODR & (1 << 0));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D1, mode, GPIOE->ODR & (1 << 1));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D2, mode, GPIOE->ODR & (1 << 2));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D3, mode, GPIOE->ODR & (1 << 3));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D4, mode, GPIOE->ODR & (1 << 4));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D5, mode, GPIOE->ODR & (1 << 5));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D6, mode, GPIOE->ODR & (1 << 6));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_D7, mode, GPIOE->ODR & (1 << 7));
  if(error) { return ERROR_UNEXPECTED(error); }
  
  return ERROR_NONE;
}


const RGB565_t Red    = { .R = 0x1F, .G = 0,    .B = 0};
const RGB565_t Green  = { .R = 0,    .G = 0x3F, .B = 0};
const RGB565_t Blue   = { .R = 0,    .G = 0,    .B = 0x1F};
const RGB565_t White  = { .R = 0x1F, .G = 0x3F, .B = 0x1F};
const RGB565_t Black  = { .R = 0,    .G = 0,    .B = 0};
const RGB565_t Gray   = { .R = 0x0F, .G = 0x1F, .B = 0x0F};

int Lcd_Initialize(void)
{
  int error;
  
  error = Lcd_SetMode(false);
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_CS, GPIO_MODE_OUTPUT, true);
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_DC, GPIO_MODE_OUTPUT, true);
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_WR, GPIO_MODE_OUTPUT, true);
  if(error) { return ERROR_UNEXPECTED(error); }
  
  error = Gpio_Initialize(GPIO_ID_TFT_RD, GPIO_MODE_OUTPUT, true);
  if(error) { return ERROR_UNEXPECTED(error); }
      
  error = Gpio_Set(GPIO_ID_TFT_CS, false);
  error = Lcd_SetMode(true);
  
  Lcd_Write(0x11, true); //exit SLEEP mode
  
  Lcd_Write(0x36, true);
  Lcd_Write(0x00, false); // MADCTL: memory data access control
    
  Lcd_Write(0x3A, true);
  Lcd_Write(0x65, false); // COLMOD: Interface Pixel format
  
  Lcd_Write(0xB2, true);
  Lcd_Write(0x0C, false);
  Lcd_Write(0x0C, false);
  Lcd_Write(0x00, false);
  Lcd_Write(0x33, false);
  Lcd_Write(0x33, false); // PORCTRK: Porch setting
  
  Lcd_Write(0xB7, true);
  Lcd_Write(0x35, false); // GCTRL: Gate Control
  
  Lcd_Write(0xBB, true);
  Lcd_Write(0x2B, false); // VCOMS: VCOM setting
  
  Lcd_Write(0xC0, true);
  Lcd_Write(0x2C, false); // LCMCTRL: LCM Control
  
  Lcd_Write(0xC2, true);
  Lcd_Write(0x01, false);
  Lcd_Write(0xFF, false); // VDVVRHEN: VDV and VRH Command Enable
  
  Lcd_Write(0xC3, true);
  Lcd_Write(0x11, false); // VRHS: VRH Set
  
  Lcd_Write(0xC4, true);
  Lcd_Write(0x20, false); // VDVS: VDV Set
  
  Lcd_Write(0xC6, true);
  Lcd_Write(0x0F, false); // FRCTRL2: Frame Rate control in normal mode
  
  Lcd_Write(0xD0, true);
  Lcd_Write(0xA4, false);
  Lcd_Write(0xA1, false); // PWCTRL1: Power Control 1
  
  Lcd_Write(0xE0, true);
  Lcd_Write(0xD0, false);
  Lcd_Write(0x00, false);
  Lcd_Write(0x05, false);
  Lcd_Write(0x0E, false);
  Lcd_Write(0x15, false);
  Lcd_Write(0x0D, false);
  Lcd_Write(0x37, false);
  Lcd_Write(0x43, false);
  Lcd_Write(0x47, false);
  Lcd_Write(0x09, false);
  Lcd_Write(0x15, false);
  Lcd_Write(0x12, false);
  Lcd_Write(0x16, false);
  Lcd_Write(0x19, false); // PVGAMCTRL: Positive Voltage Gamma control
  
  Lcd_Write(0xE1, true);
  Lcd_Write(0xD0, false);
  Lcd_Write(0x00, false);
  Lcd_Write(0x05, false);
  Lcd_Write(0x0D, false);
  Lcd_Write(0x0C, false);
  Lcd_Write(0x06, false);
  Lcd_Write(0x2D, false);
  Lcd_Write(0x44, false);
  Lcd_Write(0x40, false);
  Lcd_Write(0x0E, false);
  Lcd_Write(0x1C, false);
  Lcd_Write(0x18, false);
  Lcd_Write(0x16, false);
  Lcd_Write(0x19, false); // NVGAMCTRL: Negative Voltage Gamma control
  
  Lcd_Freeze();
  Lcd_FillRectangle(0, 0, 240, 320, Black);
  Lcd_Unfreeze();
  
  return ERROR_NONE;
}

void Lcd_FillRectangle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t width, uint_fast16_t height, RGB565_t color)
{
  Lcd_SetPosition(x, x + width - 1, y, y + height - 1);
  
  uint8_t msb = ((*((uint16_t *)&color)) >> 8);
  uint8_t lsb =  (*((uint16_t *)&color));
  uint_fast32_t count = width * height;
  
  Lcd_Write(0x2C, true);
  for(uint_fast32_t i = 0; i < count; ++i)
  {
    Lcd_Write(msb, false);
    Lcd_Write(lsb, false);
  }
}

void Lcd_Draw(uint_fast16_t x, uint_fast16_t y, uint_fast16_t width, uint_fast16_t height, RGB565_t const * data)
{
  Lcd_SetPosition(x, x + width - 1, y, y + height - 1);
  
  uint_fast32_t count = width * height;
  
  Lcd_Write(0x2C, true);
  for(uint_fast32_t i = 0; i < count; ++i)
  {
    Lcd_WritePixel(data[i]);
  }
}

typedef struct Glyph_s {
  uint16_t Width;
  uint16_t Height;
  const uint8_t * Data;
} Glyph_t;

extern const Glyph_t Font[];

int Lcd_WriteCharacter(uint_fast16_t * px, uint_fast16_t y, char character, RGB565_t foreground, RGB565_t background)
{
  if(character == ' ')
  {
    *px += 6;
    return 0;
  }
  if(character == '\t')
  {
    *px += 10;
    return 0;
  }
  if(character < '!' || character > '~')
  {
    return ERROR_INVALID_ARGUMENT;
  }
  
  uint_fast16_t x = *px;
  
  const Glyph_t * glyph = &Font[character - '!'];
  Lcd_SetPosition(x, x + glyph->Width - 1, y, y + glyph->Height - 1);
  
  uint_fast32_t count = glyph->Width * glyph->Height;
  
  Lcd_Write(0x2C, true);
  uint_fast8_t byteIndex = 0;
  uint_fast8_t byte = 0;
  uint_fast8_t bitIndex = 0;
  for(int i = 0; i < count; ++i)
  {
    if(bitIndex == 0)
    {
      byte = glyph->Data[byteIndex];
      ++byteIndex;
    }
    
    if(byte & 1)
    {
      Lcd_WritePixel(foreground);
    }
    else
    {
      Lcd_WritePixel(background);
    }
    
    byte >>= 1;
    if(bitIndex == 7)
    {
      bitIndex = 0;
    }
    else
    {
      ++bitIndex;
    }
  }
  *px = x+glyph->Width + 2;
  return 0;
}

int Lcd_WriteString(uint_fast16_t x, uint_fast16_t y, const char * string, RGB565_t foreground, RGB565_t background)
{
  for(int i = 0; string[i] != 0; ++i)
  {
    Lcd_WriteCharacter(&x, y, string[i], foreground, background);
  }
  
  return 0;
}

void Lcd_Freeze(void)
{
  Lcd_Write(0x28, true); // Display Off
}

void Lcd_Unfreeze(void)
{
  Lcd_Write(0x29, true); // Display On
}
