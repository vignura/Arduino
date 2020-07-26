/*
 * SpeedGate.c
 *
 * Created: 11-08-2018 09:37:08 PM
 * Author : Vignesh
 */ 

#include "main.h"

static S_TimeVal startTime = {0};
static S_TimeVal diff = {0};
static S_TimeVal endTime = {0};
static float ftime_us = 0;
static float fspeed = 0;
//static char arrcMsg[32] = {0};

int main(void)
{
	arduino_init();
	
	while (1)
	{
		MeasureSpeed();
	}
}


int8_t arduino_init()
{
	int8_t i8RetVal = 0;
	
	cli();
	
	UART_init(&USART0, 1000000UL);
	
	ADC_init();
	
	Timer1_init();
	
	sei();
	
	return i8RetVal;
}

int8_t MeasureSpeed()
{
	if(ReadADC_8bit() <= LED_BLOCKED_THD)
	{
		getTime(&startTime);
		
		while(1)
		{
			// for debugging
			// print_num(ReadADC_8bit());
			// print("\n", 1);
			
			if(ReadADC_8bit() > LED_BLOCKED_THD)
			{
				break;
			}
			// _delay_us(1);
		}
		
		getTime(&endTime);
		getDiffTime(&startTime, &endTime, &diff);
		ftime_us = convToMicros(&diff);
		fspeed = (CG_PROJECTILE_LEN_MM * 1000 / ftime_us);  
		
		print("Speed: ", 7);
		print_float(fspeed);
		print("m/s\tTime: ", 10);
		print_float(ftime_us);
		print("us\n", 3);
		
		// sprintf(arrcMsg, "Time: %f\tSpeed: %f", ftime_us, fspeed);
		// print(arrcMsg, strlen(arrcMsg) + 1);
		
		// print("Time: ", 6);
		// print_num(diff.u32OvfCount);
		// print("\t", 1);
		// print_num(diff.u16TCnt);
		// print("\n", 1);	
	}
	
	return 0;
}