/*
 * stdio_uart_driver.h
 *
 *  Created on: Feb 26, 2025
 *      Author: nimsgernd
 */

#ifndef APPLICATION_USER_CORE_STDIO_UART_DRIVER_H_
#define APPLICATION_USER_CORE_STDIO_UART_DRIVER_H_

#include "main.h"

#include <inttypes.h>
#include <stdio.h>

// syscalls overrides
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);

#endif /* APPLICATION_USER_CORE_STDIO_UART_DRIVER_H_ */