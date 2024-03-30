//
// uart.h
//
// Created on: Nov 12, 2023
//     Author: Dmitry Murashov (d <DOT> murashov <AT> geoscan <DOT> aero)
//

#include <stddef.h>
#include <stdint.h>

#ifndef COMMON_HAL_UART_H_
#define COMMON_HAL_UART_H_

/// \brief Configures a selected UART
/// \returns >0, if successful, <=0 otherwise
int uartConfigure(uint8_t aUartNumber, uint32_t aBaudrate);

/// \brief Prints a string into UART output
/// \returns >0, if successful, <= otherwise
int uartTryPuts(uint8_t aUartNumber, const char *aOutput);

int uartTryPutsLen(uint8_t aUartNumber, const char *aOutput, size_t aOutputLen);

void uartBusyWaitForWritten(uint8_t aUartNumber);

#endif // COMMON_HAL_UART_H_
