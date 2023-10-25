#include "states.h"
#include "Functions.h"
#include "Platform.h"
#include "Gpio.h"
#include "Adc.h"

#define Triggered false
void State_Triggered_Enter(void)
{
	Gpio_Set(GPIO_ID_LED5, false);
	Timer_Start(Triggered);
}

State_t State_Triggered(void)
{
	if(switchState())
	{
		return STATE_IDLE;
	}
	else {
		return STATE_TRIGGERED;
	}
}

void State_Triggered_Exit(void)
{
	BackupToArray();
	DrawValues();
	Gpio_Set(GPIO_ID_LED5, true);
}
