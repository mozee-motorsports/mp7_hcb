/*
 * stdio_uart_driver.c
 *
 *  Created on: Feb 26, 2025
 *      Author: nimsgernd
 */

#include "stdio_uart_driver.h"
#include "stm32h7xx_hal.h"
 
//Make sure you set #define STDIO_UART_HANDEL as some huartx in main.h
extern UART_HandleTypeDef STDIO_UART_HANDEL;
 
int _read(int file, char *ptr, int len)
{
	int bytesRead = 0;
 
	for (int i = 0; i < len; i++)
	{
		bytesRead++;
 
		HAL_UART_Receive(&STDIO_UART_HANDEL, (uint8_t*) ptr, 1, -1);
 
		if (*ptr == '\r')
		{
			HAL_UART_Transmit(&STDIO_UART_HANDEL, (uint8_t*) ptr, 1, -1);
			*ptr = '\n';
			HAL_UART_Transmit(&STDIO_UART_HANDEL, (uint8_t*) ptr, 1, -1);
			break;
		}
		else
		{
			HAL_UART_Transmit(&STDIO_UART_HANDEL, (uint8_t*) ptr, 1, -1);
		}
 
		ptr++;
	}
 
	return bytesRead;
}
 
int _write(int file, char *ptr, int len)
{
	for (int i = 0; i < len; i++)
	{
		HAL_UART_Transmit(&STDIO_UART_HANDEL, (uint8_t*) ptr++, 1, -1);
	}
 
	return len;
} 