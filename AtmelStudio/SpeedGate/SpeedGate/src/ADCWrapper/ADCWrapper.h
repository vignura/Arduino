/*
 * ADCWrapper.h
 *
 * Created: 10-08-2018 09:41:49 PM
 *  Author: Vignesh
 */ 


#ifndef ADCWRAPPER_H_
#define ADCWRAPPER_H_

#include "adc.h"

int8_t ADC_init(void);
uint16_t ReadADC(void);
uint8_t ReadADC_8bit(void);


#endif /* ADCWRAPPER_H_ */