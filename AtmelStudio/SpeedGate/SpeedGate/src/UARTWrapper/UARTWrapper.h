/*
 * UARTWrapper.h
 *
 * Created: 10-08-2018 08:15:10 PM
 *  Author: Vignesh
 */ 


#ifndef UARTWRAPPER_H_
#define UARTWRAPPER_H_

#define  F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart_mega.h"

#define UART_ERROR_BASE					-100
#define UART_E_NULL_PTR_ACCESS			(UART_ERROR_BASE + 1)

#define print(buff, size)		UART_Transmit(&USART0, (uint8_t *)buff, size)
#define print_num(num)				_print_num(&USART0, (uint32_t)num)
#define print_float(fnum)			_print_float(&USART0, (float)fnum, 3)

bool UART_init(USART_t *usart, uint32_t u32BaudRate);
uint8_t UART_Transmit(USART_t *usart, uint8_t *u8buff, uint8_t u8Size);
uint8_t UART_Receive(USART_t *usart, uint8_t *u8buff, uint8_t u8Size);
uint8_t _print_num(USART_t *usart, uint32_t u32Num);
int8_t _print_float(USART_t *usart, float fnum, int8_t i8point);


#endif /* UARTWRAPPER_H_ */