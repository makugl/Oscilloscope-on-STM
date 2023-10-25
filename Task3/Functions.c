#include "Functions.h"
#include "Platform.h"
#include "Gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include "Adc.h"

/* IDEL STATE */
#define ParamAmountX 5
#define ParamAmountY 5

static Resolution_t SecParams[ParamAmountX] = {{100, "ms/DV"},{200, "ms/DV"},{500, "ms/DV"}, {750, "ms/DV"}, {1000, "ms/DV"}};
static Resolution_t VoltParams[ParamAmountY] = {{100 ,"mV/DV"},{200 ,"mV/DV"},{300 ,"mV/DV"},{400 ,"mV/DV"}, {500 ,"mV/DV"}};

//Variables to avoid "Prellern"
static bool USER0pressed = false;
static bool USER1pressed = false;
static bool WAKEUPpressed = false;

//default Parameters
static int currentSecRes = 0;
static int currentVoltRes = 0;

#define MaxParamLen 10
//Trigger-level in [%]
static int Triggerlevel = 50;
void WriteTrigger(void);

/* TIMER */
static bool switchStates = false;
static bool stateSampling = false;


/* Buffer */
static Buffer_t SampleBuffer;
static size_t const BufferLen = 114;
static double Samples[BufferLen];


static void resetArray(double * data)
{
	for(size_t i = 0; i < BufferLen; i++)
	{
		data[i] = 0.0;
	}
}

void resetBuffer(void)
{
	resetArray(Samples);
	Buffer_Initialize(&SampleBuffer, Samples, BufferLen);
}

/* ARRAY */
static double SamplesArray[BufferLen];

//======================
/* GET Functions */
Resolution_t GetVoltRes(void)
{
	return VoltParams[currentVoltRes];
}

Resolution_t GetSecRes(void)
{
	return SecParams[currentSecRes];
}

bool switchState(void)
{
	return switchStates;
}

//=========================
/* DRAW GRID */
void DrawGridLines(void)
{
	int xPixel = 12;
	int yPixel = 54;

	while(yPixel <= 320)
	{
		while(xPixel <= 240)
		{
			Lcd_FillRectangle(xPixel, yPixel, 1, 1, Black);
			xPixel = xPixel + 2;
		}
		xPixel = 12;
		yPixel = yPixel + 38;
	}
}

void DrawGridColoums(void)
{
	int xPixel = 12;
	int yPixel = 16;

	while(xPixel <= 240)
	{
		while(yPixel <= 320)
		{
			Lcd_FillRectangle(xPixel, yPixel, 1, 1, Black);
			yPixel = yPixel + 2;
		}
		yPixel = 16;
		xPixel = xPixel + 38;
	}
}


void DrawGrid(void)
{
	Lcd_FillRectangle(1, 1, 240, 320, White);
	Lcd_FillRectangle(1, 16, 240, 1, Black);
	Lcd_FillRectangle(126, 16, 1, 320, Black);
	Lcd_FillRectangle(12, 168, 240, 1, Black);
		
	WriteRes(10, 1, SecParams[currentSecRes], Black, White);
	WriteRes(130, 1, VoltParams[currentVoltRes],Black, White);
	WriteTrigger();

	DrawGridLines();
	DrawGridColoums();
}

//=========================================
/* BUTTONS */
bool KeyPressed(unsigned int PinID)
{
	bool pressed = false;
	Gpio_Get(PinID,&pressed);
	return pressed > 0;
}

static uint8_t SelectedState = 0;

//=========================================
/* IDLE-STATE */
//Selcet Resolution and Triggerlevel
void SetResolution()
{
	Resolution_t OldSecRes = SecParams[currentSecRes];
	Resolution_t OldVoltRes = VoltParams[currentVoltRes];
	
	while(!(KeyPressed(GPIO_ID_BUTTON_USER1) && KeyPressed(GPIO_ID_BUTTON_WAKE_UP)))
	{
		//set value to be edited
		if(KeyPressed(GPIO_ID_BUTTON_WAKE_UP))
		{
			switch(SelectedState)
			{
				case 0:
					if(!WAKEUPpressed)
					{
						Lcd_FillRectangle(1, 1, 126, 16, Black);
						Lcd_FillRectangle(126, 1, 240, 15, White);
						Lcd_FillRectangle(1, 17, 11, 303, White);

						WriteRes(10, 1, SecParams[currentSecRes], White, Black);
						WriteRes(130, 1, VoltParams[currentVoltRes], Black, White);
						WriteTrigger();
						SelectedState = 1;
					}
					WAKEUPpressed = true;
					break;

				case 1:
					if(!WAKEUPpressed)
					{
						Lcd_FillRectangle(126, 1, 240, 16, Black);
						Lcd_FillRectangle(1, 1, 126, 15, White);
						Lcd_FillRectangle(1, 17, 11, 303, White);

						WriteRes(10, 1, SecParams[currentSecRes], Black, White);
						WriteRes(130, 1, VoltParams[currentVoltRes], White, Black);
						WriteTrigger();
						SelectedState	= 2;
					}
					WAKEUPpressed = true;
					break;

				case 2:
					if(!WAKEUPpressed)
					{
						Lcd_FillRectangle(126, 1, 240, 15, White);
						Lcd_FillRectangle(1, 17, 11, 303, Black);

						WriteRes(130, 1, VoltParams[currentVoltRes], Black, White);
						WriteTrigger();
						SelectedState = 0;	
						if(SecParams[currentSecRes].Parameter != OldSecRes.Parameter || VoltParams[currentVoltRes].Parameter != OldVoltRes.Parameter)
						{
							OldSecRes = SecParams[currentSecRes];
							OldVoltRes = VoltParams[currentVoltRes];
							DrawGrid();
							DrawValues();
						}	
					}
					WAKEUPpressed = true;
					break;
			}
		}
		//decrease resolution value
		else if(KeyPressed(GPIO_ID_BUTTON_USER0))
		{
			if(SelectedState == 2 && currentVoltRes > 0 && !USER0pressed)
			{
				currentVoltRes--;
				Lcd_FillRectangle(126, 1, 240, 15, Black);
				WriteRes(130, 1, VoltParams[currentVoltRes], White, Black);
			}
			else if(SelectedState == 1 && currentSecRes > 0 && !USER0pressed)
			{
				currentSecRes--;
				Lcd_FillRectangle(1, 1, 126, 15, Black);
				WriteRes(10, 1, SecParams[currentSecRes], White, Black);
			}
			else if(Triggerlevel >= 5 && SelectedState == 0 && !USER0pressed)
				{
					Lcd_FillRectangle(1, 17, 11, 303, Black);
					Triggerlevel = Triggerlevel - 5;
					WriteTrigger();
				}
			USER0pressed = true;
		}
		//increase Resolution value
		else if(KeyPressed(GPIO_ID_BUTTON_USER1))
		{
			if(SelectedState == 2 && currentVoltRes < (ParamAmountY - 1) && !USER1pressed)
			{
				currentVoltRes++;
				Lcd_FillRectangle(126, 1, 240, 15, Black);
				WriteRes(130, 1, VoltParams[currentVoltRes], White, Black);
			}
			else if(SelectedState == 1 && currentSecRes < (ParamAmountX - 1) && !USER1pressed)
			{
				currentSecRes++;
				Lcd_FillRectangle(1, 1, 126, 15, Black);
				WriteRes(10, 1, SecParams[currentSecRes], White, Black);
			}
			else if(Triggerlevel <= 95 && SelectedState == 0 && !USER1pressed)
			{
				Lcd_FillRectangle(1, 17, 11, 303, Black);
				Triggerlevel = Triggerlevel + 5;
				WriteTrigger();
			}
			USER1pressed = true;
		}
		else{USER0pressed = false; USER1pressed = false; WAKEUPpressed = false;}
	}
}

//Write ADC-Value to LCD
void WriteRes(uint_fast16_t x, uint_fast16_t y, Resolution_t Resolution, RGB565_t foreground, RGB565_t background)
{
	char toPrint[MaxParamLen];
	snprintf(toPrint, 32, "%*d%s", 6, Resolution.Parameter, Resolution.Unit);
	Lcd_WriteString(x, y, toPrint, foreground, background);
}

//Write Triggervlaue to LCD
void WriteTrigger(void)
{
	uint16_t dispTrigger = 0;
	dispTrigger = 318 - (300 * Triggerlevel) / 100;
	Lcd_FillRectangle(1,dispTrigger, 11, 1, Blue);
}

//===================================================
/* SAMPLING - STATE */
void BackupToArray (void)
{
	resetArray(SamplesArray);
	for(size_t i = 0; i<= BufferLen; i++)
	{
		Buffer_GetByte(&SampleBuffer,&SamplesArray[i]);
	}
}

//===================================================
/* TRIGGERED - STATE */
static uint_fast16_t VoltToPixel(double Volt)
{
		const uint_fast16_t maxHeigh = 17;
		const uint_fast16_t offset = 320;
		const uint_fast16_t voltSum = Volt / (VoltParams[currentVoltRes].Parameter * 8.0) *  (offset - maxHeigh);
		const uint_fast16_t result = offset - voltSum;
	
		return (result > maxHeigh ? result : maxHeigh);
}

void DrawValues(void)
{
	for(size_t i = 0; i < BufferLen; i++)
	{
		Lcd_FillRectangle(i*2 + 12, VoltToPixel(SamplesArray[i]),2,1,Red);
	}
}

//===================================================
/* TIMER */

#define prescaler 8000
void Timer_Initialize(void)
{
	//enable clock for timer 7
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	//update interrupt enable
	TIM7->DIER = TIM_DIER_UIE;

	//set Prescaler
	TIM7->PSC = (prescaler - 1);
	TIM7->ARR = (2);
	
	//activate interrupt
	NVIC_EnableIRQ(TIM7_IRQn);
}

static size_t index = BufferLen/2;

void TIM7_IRQHandler(void)
{
	//interrupt bit set?
	if(TIM7->SR & TIM_SR_UIF)
	{
		//reset interrupt flag
		TIM7->SR &= ~(TIM_SR_UIF);
		double ADC_Value = 0.0;
 		Adc_Convert(ADC_ID_POTENTIOMETER, &ADC_Value, false);
		
		double TriggerValue = VoltParams[currentVoltRes].Parameter * 8.0 * Triggerlevel / 100.0;
		
		if(stateSampling)
		{
			if(Buffer_PutByte(&SampleBuffer,(ADC_Value * 1000)) != 0)
			{
				/* ERROR - STATE */
				Gpio_Set(GPIO_ID_LED3, false);
				Gpio_Set(GPIO_ID_LED4, false);
				Gpio_Set(GPIO_ID_LED5, false);
			}
			
			if(ADC_Value * 1000 >= TriggerValue)
			{
				switchStates = true;
				//stop timer
				TIM7->CR1 &= (~TIM_CR1_CEN);
			}	
			else{switchStates = false;}
		}
		else
		{
			if(index < BufferLen)
			{
				if(ADC_Value * 1000 >= TriggerValue) //above Triggerlevel
				{
					if(Buffer_PutByte(&SampleBuffer,(ADC_Value * 1000)) != 0)
					{
						//*ERROR - STATE *//*
						Gpio_Set(GPIO_ID_LED3, false);
						Gpio_Set(GPIO_ID_LED4, false);
						Gpio_Set(GPIO_ID_LED5, false);
					}
					index++;
				}
				switchStates = false;
			}
			else 
			{
				switchStates = true;
				TIM7->CR1 &= (~TIM_CR1_CEN);
			}
		}
	}
}

#define AmountofSamples 114
void Timer_Start(bool const State)
{
	switchStates = false;
	stateSampling = State;
	index = BufferLen/2;;
	TIM7->ARR = (6 * SecParams[currentSecRes].Parameter) / AmountofSamples;
	TIM7->CR1 |= TIM_CR1_CEN;
}
