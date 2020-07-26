/*
 * main.h
 *
 * Created: 10-08-2018 09:39:00 PM
 *  Author: Vignesh
		 */ 


#ifndef MAIN_H_
#define MAIN_H_
	
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#define LED_BLOCKED_THD				180
#define CG_PROJECTILE_LEN_MM		26.0f

#include "UARTWrapper/UARTWrapper.h"
#include "ADCWrapper/ADCWrapper.h"
#include "TimerWrapper/TimerWrapper.h"

int8_t arduino_init();
int8_t MeasureSpeed();

#endif /* MAIN_H_ */