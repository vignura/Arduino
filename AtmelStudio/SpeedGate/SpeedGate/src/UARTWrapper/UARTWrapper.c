/*
 * UARTWrapper.c
 *
 * Created: 10-08-2018 08:14:52 PM
 *  Author: Vignesh
 */ 

#include "UARTWrapper.h"

bool UART_init(USART_t *usart, uint32_t u32BaudRate)
{
	bool bRetVal = false;
	
	usart_rs232_options_t S_UsartOptions = {0};
	
	S_UsartOptions.baudrate = u32BaudRate;
	S_UsartOptions.charlength = USART_CHSIZE_8BIT_gc;
	S_UsartOptions.paritytype = USART_PMODE_DISABLED_gc;
	S_UsartOptions.stopbits = false;
	
	bRetVal = usart_init_rs232(usart, &S_UsartOptions);
	
	return bRetVal;
}

uint8_t UART_Transmit(USART_t *usart, uint8_t *u8buff, uint8_t u8Size)
{
	uint8_t u8Index = 0;
	
	if(u8buff == NULL)
	{
		return UART_E_NULL_PTR_ACCESS;
	}
	
	for(u8Index = 0; u8Index < u8Size; u8Index++)
	{
		usart_putchar(usart, u8buff[u8Index]);
	}
	
	return u8Index;
}

uint8_t UART_Receive(USART_t *usart, uint8_t *u8buff, uint8_t u8Size)
{
	uint8_t u8Index = 0;
	
	if(u8buff == NULL)
	{
		return UART_E_NULL_PTR_ACCESS;
	}
	
	for(u8Index = 0; u8Index < u8Size; u8Index++)
	{
		u8buff[u8Index] = usart_getchar(usart);
	}
	
	return u8Index;
}

uint8_t _print_num(USART_t *usart, uint32_t u32Num)
{
	uint8_t u8zero = '0';
	uint8_t u8buff[12] = {0};
	uint8_t u8Cnt = 0; 
	uint8_t u8Index = 0;
	
	if(u32Num == 0)
	{
		usart_putchar(usart, u8zero);	
	}
	else
	{	
		while(u32Num > 0)
		{
			u8buff[u8Index] = (u32Num % 10) + u8zero;
			u32Num = u32Num / 10;
			u8Index++;
			u8Cnt++;
		}
	
		for(u8Index = u8Cnt; u8Index > 0; u8Index--)
		{
			usart_putchar(usart, u8buff[u8Index -1]);
		}
	}
	
	return u8Cnt;
}

int8_t _print_float(USART_t *usart, float fnum, int8_t i8point)
{
	uint8_t u8zero = '0';
	uint8_t u8buff[8] = {0};
	int32_t i32num = (int32_t)fnum;
	int8_t i8Index = 0;
	
	fnum -= i32num;
	u8buff[0] = '.';
	
	for(i8Index = 1; (i8Index < i8point + 1) && (i8Index < 8); i8Index++)
	{
		fnum *= 10;
		u8buff[i8Index] = ((int32_t)fnum % 10) + u8zero; 	
	}
	
	_print_num(usart, i32num);
	UART_Transmit(usart, u8buff, i8Index);
	
	return i8Index;
}