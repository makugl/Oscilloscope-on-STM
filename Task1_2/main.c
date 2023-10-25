//===================================
//name: Manuel Kugler
//date: 09.12.2018
//description: Task 1 and 2
//===================================
#include <stm32f072xb.h>
#include "Platform.h"
#include "Adc.h"
#include "Lcd.h"
#include "GPIO.h"

#include <stdio.h>

//define Colours
static const RGB565_t blue = {0x19, 0x12, 0xB};
static const RGB565_t orange = {0x5, 0xF, 0x1C};

int main(void)
{	
  /* GPIOC Ports Clock Enable */
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	//first task
	Adc_Initialize();
	
	double ConvertedValue = 0.0;
	double RawValue  = 0.0;
	
  Adc_Convert(ADC_ID_VREFINT, &ConvertedValue, false);
  Adc_Convert(ADC_ID_VREFINT, &RawValue, true);
	
  Adc_Convert(ADC_ID_TEMPERATURE, &ConvertedValue, false);
  Adc_Convert(ADC_ID_TEMPERATURE, &RawValue, true);
	
  Adc_Convert(ADC_ID_POTENTIOMETER, &ConvertedValue, false);
  Adc_Convert(ADC_ID_POTENTIOMETER, &RawValue, true);
	
	
	//second task
	Lcd_Initialize();
	
	Lcd_FillRectangle(1, 1, 240, 320, Black);
	Lcd_FillRectangle(2, 2, 237, 317, White);
	
	Lcd_FillRectangle(25, 85, 200, 100, Black);
	Lcd_FillRectangle(26, 86, 198, 98, White);
	
	Lcd_FillRectangle(40, 100, 170, 30, orange);
	Lcd_FillRectangle(40, 140, 170, 30, blue);
	
	Lcd_WriteString(85, 108, "HSD 2018", White, orange);
	Lcd_WriteString(73, 148, "S1710306018", White, blue);
	
  while(1)
	{ 
		
	}
}
