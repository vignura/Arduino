/*
 * TimerWrapper.c
 *
 * Created: 11-08-2018 09:53:38 PM
 *  Author: Vignesh
 */ 

#include "TimerWrapper.h"

// global variable to store time
uint32_t g_u32OvfCount = 0;

ISR(TIMER1_OVF_vect)
{
	g_u32OvfCount++;
}

int8_t Timer1_init()
{
	uint8_t u8RetVal = 0;
	
	// clear global overflow count
	g_u32OvfCount = 0;
	
	// clear timer
	TCNT1 = 0;
	
	// set clock prescalar to 1
	TCCR1B |= (1 << CS10);
	
	// enable overflow interrupt
	TIMSK1 |= (1 << TOIE1);
	
	return u8RetVal;
}

uint16_t TIM16_ReadTCNT1(void)
{
	uint8_t u8Sreg;
	uint16_t u16Count;
	
	// Save global interrupt flag
	u8Sreg = SREG;
	
	// Disable interrupts
	cli();
	
	// Read TCNT1 into u16Count
	u16Count = TCNT1;
	
	// Restore global interrupt flag
	SREG = u8Sreg;
	
	return u16Count;
}

int8_t getTime(S_TimeVal *out_Time)
{
	int8_t i8RetVal = 0;
	
	if(out_Time == NULL)
	{
		return -1;
	}
	
	out_Time->u16TCnt = TIM16_ReadTCNT1();
	out_Time->u32OvfCount = g_u32OvfCount;
	
	return i8RetVal;	
}

int8_t getDiffTime(S_TimeVal *in_start, S_TimeVal *in_stop, S_TimeVal *out_diff)
{
	int8_t i8RetVal = 0;
	
	if((in_start == NULL) || (in_stop == NULL) || (out_diff == NULL))
	{
		return -1;
	}
	
	out_diff->u32OvfCount = in_stop->u32OvfCount - in_start->u32OvfCount;
	
	if(in_stop->u16TCnt >= in_start->u16TCnt)
	{
		out_diff->u16TCnt = in_stop->u16TCnt - in_start->u16TCnt;
	}
	else
	{
		out_diff->u32OvfCount--;
		out_diff->u16TCnt = (TIMER_16BIT_MAX_VAL - in_start->u32OvfCount) + in_stop->u16TCnt;
	}
	
	return i8RetVal;
}

float convToMicros(S_TimeVal *S_time)
{
	float time_us = 0;
	
	time_us = (S_time->u32OvfCount * TIMER1_OVERFLOW_TIME_US);
	time_us += (((float)S_time->u16TCnt / (float)TIMER_16BIT_MAX_VAL) * TIMER1_OVERFLOW_TIME_US);
	
	return time_us;
}