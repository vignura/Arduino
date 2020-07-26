/*
 * ADCWrapper.c
 *
 * Created: 10-08-2018 09:41:33 PM
 *  Author: Vignesh
 */ 

#include "ADCWrapper.h"

int8_t ADC_init(void)
{
	int8_t i8RetVal = 0;
	
	adc_init(ADC_PRESCALER_DIV8);
	
	return i8RetVal;
}

uint16_t ReadADC(void)
{
	return adc_read_10bit(ADC_MUX_ADC0, ADC_VREF_AVCC);
}

uint8_t ReadADC_8bit(void)
{
	return adc_read_8bit(ADC_MUX_ADC0, ADC_VREF_AVCC);
}