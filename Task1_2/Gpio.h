#ifndef GPIO_H
#define GPIO_H


// Include C standard library header files.
#include <stdbool.h>


/*******************************************************************************
Defines the available modes for GPIO pins.
*******************************************************************************/
typedef enum Gpio_Mode_e {
   GPIO_MODE_OUTPUT,
   GPIO_MODE_OUTPUT_OPEN_DRAIN,
   GPIO_MODE_ALTERNATE_FUNCTION,
   GPIO_MODE_ALTERNATE_FUNCTION_OPEN_DRAIN,
   GPIO_MODE_INPUT_ANALOG,
   GPIO_MODE_INPUT,
   GPIO_MODE_INPUT_PULL_DOWN,
   GPIO_MODE_INPUT_PULL_UP
} Gpio_Mode_t;

/*******************************************************************************
Initializes the specified GPIO pin.
id: The ID of the GPIO pin.
mode: The mode in which the GPIO pin will be used.
value: The output value of the GPIO pin; ignored if the GPIO pin is used as input pin.
  false: The pin output is set to low.
  true: The pin output is set to high. 
returns: Zero if the operation was successfull, an error code otherwise.
The GPIO pin is not intialized if an error code is returned.
*******************************************************************************/
int Gpio_Initialize(unsigned int id, Gpio_Mode_t mode, bool value);

/*******************************************************************************
Sets the value of the specified GPIO pin.
id: The ID of the GPIO pin.
value: The output value of the GPIO pin.
  false: The pin output is set to low.
  true: The pin output is set to high. 
returns: Zero if the operation was successfull, an error code otherwise.
The pin output is not changed if an error code is returned.
*******************************************************************************/
int Gpio_Set(unsigned int id, bool value);

/*******************************************************************************
Gets the value of the specified GPIO pin.
id: The ID of the GPIO pin.
value: The value of the pin.
  false: The pin is currently low.
  true: The pin is currently high. 
returns: Zero if the operation was successfull, an error code otherwise.
*******************************************************************************/
int Gpio_Get(unsigned int id, bool * value);


#endif // GPIO_H

