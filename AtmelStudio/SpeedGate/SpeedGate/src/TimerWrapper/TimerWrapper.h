/*
 * TimerWrapper.h
 *
 * Created: 11-08-2018 09:53:50 PM
 *  Author: Vignesh
 */ 


#ifndef TIMERWRAPPER_H_
#define TIMERWRAPPER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#define TIMER_16BIT_MAX_VAL			65535UL
#define TIMER1_PRESCALAR			1
#define TIMER1_OVERFLOW_TIME_US		4096.0f		// us

typedef struct
{
	uint32_t u32OvfCount;			// over flow count
	uint16_t u16TCnt;				// timer count read by register
	
}S_TimeVal;


int8_t Timer1_init();
uint16_t TIM16_ReadTCNT1(void);
int8_t getDiffTime(S_TimeVal *in_start, S_TimeVal *in_stop, S_TimeVal *out_diff);
int8_t getTime(S_TimeVal *out_Time);
float convToMicros(S_TimeVal *S_time);

#endif /* TIMERWRAPPER_H_ */