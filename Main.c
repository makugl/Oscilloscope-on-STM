#include "Platform.h"
#include "Key.h"
#include "Led.h"
#include "Lcd.h"
#include "Adc.h"


#include <stdio.h>


typedef enum State_e {
  STATE_IDLE,
  STATE_SAMPLING,
  STATE_TRIGGERED
} State_t;

typedef State_t (*StateFunction_t)(void);
typedef void (*StateTransitionFunction_t)(void);

typedef struct StateFunctions_s {
  StateTransitionFunction_t Enter;
  StateFunction_t           Do;
  StateTransitionFunction_t Exit;
} StateFunctions_t;

// Move this to a separate module (States/Idle.c)!
void State_Idle_Enter(void) { Led_On(LED_ID_LED3); }
State_t State_Idle(void)    { return STATE_SAMPLING; }
void State_Idle_Exit(void)  { Led_Off(LED_ID_LED3); }

// Move this to a separate module (States/Sampling.c)!
void State_Sampling_Enter(void) { Led_On(LED_ID_LED4); }
State_t State_Sampling(void)    { return STATE_TRIGGERED; }
void State_Sampling_Exit(void)  { Led_Off(LED_ID_LED4); }

// Move this to a separate module (States/Triggered.c)!
void State_Triggered_Enter(void) { Led_On(LED_ID_LED5); }
State_t State_Triggered(void)    { return STATE_IDLE; }
void State_Triggered_Exit(void)  { Led_Off(LED_ID_LED5); }

#define STATE_COUNT 3
const StateFunctions_t States[STATE_COUNT] = {
  { State_Idle_Enter,      State_Idle,      State_Idle_Exit      },
  { State_Sampling_Enter,  State_Sampling,  State_Sampling_Exit  },
  { State_Triggered_Enter, State_Triggered, State_Triggered_Exit }
};

int main(void)
{
  Led_Initialize(LED_ID_LED3);
  Led_Initialize(LED_ID_LED4);
  Led_Initialize(LED_ID_LED5);

  Adc_Initialize();
  Lcd_Initialize();

  double value;
  if(!Adc_Convert(ADC_ID_VREFINT, &value, false))
  {
    static char string[32];
    snprintf(string, 32, "ADC: %0.2f V", value);
    bool ok = (value >= 1.2) && (value <= 1.25);
    Lcd_WriteString(10, 10, string, ok ? White : Red, Black);
  }

  State_t state = STATE_IDLE;
  while(1)
  {
    State_t nextState = States[state].Do();
    if(nextState != state)
    {
      if(States[state].Exit != NULL)
      {
        States[state].Exit();
      }

      state = nextState;

      if(States[state].Enter != NULL)
      {
        States[state].Enter();
      }
    }
  }
}
