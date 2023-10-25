#ifndef ADC_H
#define ADC_H


#include <stdint.h>
#include <stdbool.h>


int Adc_Initialize(void);
int Adc_Calibrate(uint_fast8_t * calibrationFactor);
int Adc_Enable(void);
int Adc_Disable(void);
int Adc_Convert(int id, double * value, bool raw);


#endif // ADC_H
