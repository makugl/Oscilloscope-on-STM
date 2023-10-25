#include "states.h"
#include "Platform.h"
#include "Gpio.h"
#include "Adc.h"
#include "Functions.h"

#define Sampling true
void State_Sampling_Enter(void)
{
	Gpio_Set(GPIO_ID_LED4, false);
	Timer_Start(Sampling);
}

State_t State_Sampling(void)
{
	if(KeyPressed(GPIO_ID_BUTTON_WAKE_UP))
	{
		return STATE_IDLE;
	}
	
	if(switchState())
	{
		return STATE_TRIGGERED;
	}
	else
 {
	return STATE_SAMPLING;
 } 
}


void State_Sampling_Exit(void)
{
	Gpio_Set(GPIO_ID_LED4, true);
	//stop timer
	TIM7->CR1 &= (~TIM_CR1_CEN);
}
