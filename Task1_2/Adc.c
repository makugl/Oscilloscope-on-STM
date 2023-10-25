// Include module header files.
#include "Adc.h"
#include "Gpio.h"

// Include custom header files.
#include "Platform.h"

// Include toolchain header files.
#include <stm32f072xb.h>

// Include C standard library header files.
#include <stdbool.h>
#include <stddef.h>

#define VREFINT_CAL (*((const uint16_t *)0x1FFFF7BA))
#define TS_CAL1     (*((const uint16_t *)0x1FFFF7B8))
#define TS_CAL2     (*((const uint16_t *)0x1FFFF7C2))


static uint_fast16_t VREFINT_DATA = 0x05FA;

void ADC_COMP(void)
{
  volatile uint16_t dr = ADC1->DR;
  volatile double Vdda = 0.3 * VREFINT_CAL / dr;
  volatile double T = 90.0/(TS_CAL2 - TS_CAL1) * (dr - TS_CAL1) + 30.0;
}

int Adc_Initialize()
{
  int error = Gpio_Initialize(GPIO_ID_POTENTIOMETER, GPIO_MODE_INPUT_ANALOG, false);
  if(error)
  {
    return ERROR_UNEXPECTED(error);
  }

  // Enable peripheral clock.
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;


  // ADC interrupt enable register
  // Disable all interrupts.
  ADC1->IER &= ~( ADC_ISR_AWD
                | ADC_ISR_OVR
                | ADC_ISR_EOSEQ
                | ADC_ISR_EOC
                | ADC_ISR_EOSMP
                | ADC_ISR_ADRDY );

  // ADC interrupt and status register
  // Clear all interrupt flags.
  ADC1->ISR |= ADC_ISR_AWD
            |  ADC_ISR_OVR
            |  ADC_ISR_EOSEQ
            |  ADC_ISR_EOC
            |  ADC_ISR_EOSMP
            |  ADC_ISR_ADRDY;

  // ADC configuration register 1
  uint32_t cfgr1 = ADC1->CFGR1 & ~( ADC_CFGR1_AWD1CH_Msk
                                  | ADC_CFGR1_AWD1EN_Msk
                                  | ADC_CFGR1_AWD1SGL_Msk
                                  | ADC_CFGR1_DISCEN_Msk
                                  | ADC_CFGR1_AUTOFF_Msk
                                  | ADC_CFGR1_WAIT_Msk
                                  | ADC_CFGR1_CONT_Msk
                                  | ADC_CFGR1_OVRMOD_Msk
                                  | ADC_CFGR1_EXTEN_Msk
                                  | ADC_CFGR1_EXTSEL_Msk
                                  | ADC_CFGR1_ALIGN_Msk
                                  | ADC_CFGR1_RES_Msk
                                  | ADC_CFGR1_SCANDIR_Msk
                                  | ADC_CFGR1_DMACFG_Msk
                                  | ADC_CFGR1_DMAEN_Msk);
  cfgr1 |= ADC_CFGR1_WAIT;
  ADC1->CFGR1 = cfgr1;

  // ADC configuration register 2
  uint32_t cfgr2 = ADC1->CFGR2 & ~( ADC_CFGR2_CKMODE_Msk);
  cfgr2 |= ADC_CFGR2_CKMODE_1;
  ADC1->CFGR2 = cfgr2;

  // ADC sampling time register
  uint32_t smpr = ADC1->SMPR & ~( ADC_SMPR_SMP_Msk );
  smpr |= ADC_SMPR_SMP_2 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_0;
  ADC1->SMPR |= smpr;

  // ADC channel selection register.
  uint32_t ccr = ADC->CCR & ( ADC_CCR_VBATEN_Msk
                            | ADC_CCR_TSEN_Msk
                            | ADC_CCR_VREFEN_Msk );
  ccr |= ( ADC_CCR_VBATEN
         | ADC_CCR_TSEN
         | ADC_CCR_VREFEN );
  ADC->CCR = ccr;

  NVIC_EnableIRQ(ADC1_COMP_IRQn);
  NVIC_SetPriority(ADC1_COMP_IRQn, 3);

  // ADC1->IER |= ADC_IER_EOCIE;

  Adc_Calibrate(NULL);
  Adc_Enable();

  double value;
  Adc_Convert(ADC_ID_VREFINT, &value, true);
  VREFINT_DATA = value;

  return ERROR_NONE;
}

int Adc_Calibrate(uint_fast8_t * calibrationFactor)
{
  // The internal analog calibration is kept if the ADC is disabled (ADEN=0).
  // When the ADC operating conditions change (VDDA changes are the main
  // contributor to ADC offset variations and temperature change to a
  // lesser extend), it is recommended to re-run a calibration cycle.
  // The calibration factor is lost each time power is removed from the ADC
  // (for example when the product enters STANDBY mode).
  // [RM], 13.4.1 Calibration (ADCAL), Page 230

  if((ADC1->CR & ADC_CR_ADEN) | (ADC1->CFGR1 & ADC_CFGR1_DMAEN))
  {
    return ERROR_INVALID_OPERATION;
  }

  // Start calibration.
  ADC1->CR |= ADC_CR_ADCAL;

  // Wait until calibration has been completed.
  while(ADC1->CR & ADC_CR_ADCAL);

  // Read calibration factor from DR (bits 6 to 0).
  if(calibrationFactor != NULL)
  {
    *calibrationFactor = (ADC1->DR & 0x7F);
  }

  return ERROR_NONE;
}

int Adc_Enable()
{
  // Clear the ADRDY bit in ADC_ISR register by programming this bit to 1.
  ADC1->ISR &= ~ADC_ISR_ADRDY;

  do
  {
    // Enable ADC.
    ADC1->CR |= ADC_CR_ADEN;

  // Wait until ADC is ready.
  } while(!(ADC1->ISR & ADC_ISR_ADRDY));

  return ERROR_NONE;
}

int Adc_Disable()
{
  // Stop any ongoing conversion.
  ADC1->CR |= ADC_CR_ADSTP;

  // Wait until conversion has been stopped.
  while(ADC1->CR & ADC_CR_ADSTP);

  // Disable ADC.
  ADC1->CR |= ADC_CR_ADDIS;

  // Wait until ADC is fully disabled.
  while(ADC1->ISR & ADC_ISR_ADRDY);

  // Clear the ADRDY bit in ADC_ISR register by programming this bit to 1.
  ADC1->ISR |= ADC_ISR_ADRDY;

  return ERROR_NONE;
}

int Adc_Convert(int id, double * value, bool raw)
{
  // ADC channel selection register.
  uint32_t chselr = ADC1->CHSELR & ~( ADC_CHSELR_CHSEL0_Msk
                                    | ADC_CHSELR_CHSEL1_Msk
                                    | ADC_CHSELR_CHSEL2_Msk
                                    | ADC_CHSELR_CHSEL3_Msk
                                    | ADC_CHSELR_CHSEL4_Msk
                                    | ADC_CHSELR_CHSEL5_Msk
                                    | ADC_CHSELR_CHSEL6_Msk
                                    | ADC_CHSELR_CHSEL7_Msk
                                    | ADC_CHSELR_CHSEL8_Msk
                                    | ADC_CHSELR_CHSEL9_Msk
                                    | ADC_CHSELR_CHSEL10_Msk
                                    | ADC_CHSELR_CHSEL11_Msk
                                    | ADC_CHSELR_CHSEL12_Msk
                                    | ADC_CHSELR_CHSEL13_Msk
                                    | ADC_CHSELR_CHSEL14_Msk
                                    | ADC_CHSELR_CHSEL15_Msk
                                    | ADC_CHSELR_CHSEL16_Msk
                                    | ADC_CHSELR_CHSEL17_Msk
                                    | ADC_CHSELR_CHSEL18_Msk );
  chselr |= ( 1 << id );
  ADC1->CHSELR = chselr;

  ADC1->CR |= ADC_CR_ADSTART;
  while(!(ADC1->ISR & ADC_ISR_EOC));
  uint16_t dr = ADC1->DR;

  if(raw)
  {
    *value = dr;
    return ERROR_NONE;
  }

  switch(id)
  {
    case ADC_ID_TEMPERATURE:
      *value = 90.0 / (TS_CAL2 - TS_CAL1) * (dr - TS_CAL1) + 30.0;
      break;

    default:
      *value = VREFINT_CAL * 3.3 * dr / (VREFINT_DATA * 1.0 * 0x0FFF);
      break;
  }

  return ERROR_NONE;
}
