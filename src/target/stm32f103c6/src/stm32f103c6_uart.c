//
// uart.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include "clock.h"
#include <stm32f103x6.h>

#define USBAD_STM32F103C6_USARTDIV_COEFFICIENT (16.0f)
#define USBAD_STM32F103C6_BRR_FRACTION_NBITS (4)

/// \brief Initializes RCC registers
static void configureClock();

/// \brief Input clock frequency for usart
static uint32_t getUsart1InputClockFrequency();

/// \brief see RM0008, rev. 21, p. 1136, "Fractional baudrate"
static uint32_t calculateBrrRegisterValue(uint32_t aBaudrate, uint32_t aInputFrequency);

static void configureClock()
{
	volatile RCC_TypeDef *rcc = RCC;

	rcc->APB2ENR |= RCC_APB2ENR_USART1EN;
}

static uint32_t getUsart1InputClockFrequency()
{
	return clockGetSysclkFrequency() / clockGetApb2Prescaler();
}

static uint32_t calculateBrrRegisterValue(uint32_t aBaudrate, uint32_t aInputFrequency)
{
	const float usartDivf = (float)aInputFrequency  / (float)aBaudrate / USBAD_STM32F103C6_USARTDIV_COEFFICIENT;
	const uint32_t mantissa = (uint32_t)usartDivf;
	float fractionf = usartDivf - (float)mantissa;
	uint32_t fraction;
	fractionf *= USBAD_STM32F103C6_USARTDIV_COEFFICIENT;
	fraction = (uint32_t)fractionf;

	return (mantissa << USBAD_STM32F103C6_BRR_FRACTION_NBITS) | fraction;
}

void uartUp()
{
	configureClock();
}

int uartConfigure(uint8_t aUartNumber, uint32_t aBaudrate)
{
	uint32_t uartFrequency = 0;
	volatile USART_TypeDef *usart = 0;

	switch (aUartNumber) {
		case 1: {
			usart = USART1;
			uartFrequency = getUsart1InputClockFrequency();

			break;
		}
		default:
			return 0;
	}

	// Configure baudrate
	usart->BRR = calculateBrrRegisterValue(aBaudrate, uartFrequency);

	// Enable USART, "TX empty" interrupt, "RX not empty" interrupt
	usart->CR1 |= USART_CR1_UE | USART_CR1_TXEIE | USART_CR1_RXNEIE;
}
