//
// uart.h
//
// Created on: Nov 12, 2023
//     Author: Dmitry Murashov (d <DOT> murashov <AT> geoscan <DOT> aero)
//

#include <stdint.h>

#ifndef COMMON_HAL_UART_H_
#define COMMON_HAL_UART_H_

/// \brief Configures a selected UART
/// \returns >0, if successful, <=0 otherwise
int uartConfigure(uint8_t aUartNumber, uint32_t aBaudrate);

#endif // COMMON_HAL_UART_H_
