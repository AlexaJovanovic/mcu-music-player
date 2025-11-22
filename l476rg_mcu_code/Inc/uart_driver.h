/*
 * uart_driver.h
 *
 *  Created on: Sep 26, 2025
 *      Author: Aleksa
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "stm32l4xx_hal.h"   // adjust to your MCU family
#include <stdint.h>

// Initializes USART2 for PC communication
void UART_PC_Init(void);

// Send a single character
void UART_PC_SendChar(char c);

// Send a string
void UART_PC_SendString(const char *str);

// Receive a character (blocking)
char UART_PC_ReceiveChar(void);

// Send an unsigned integer (uint32_t) as text
void UART_PC_SendUInt(uint32_t value);

#endif // UART_DRIVER_H
