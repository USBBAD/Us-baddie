//
// clock.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include "stm32f103c6_uart.h"
#include <stm32f103x6.h>
#include <stdint.h>

void clockInitialize()
{
	volatile RCC_TypeDef *rcc = RCC;
	// Enable Internal 8 MHz HSI clock source
	rcc->CR |= RCC_CR_HSION;

	// Wait until HSI is ready
	while (!(rcc->CR & RCC_CR_HSIRDY));

	// Use HSI as the SYSCLK source
	rcc->CFGR |= RCC_CFGR_SW_PLL;

	// PLLMUL Multiply PLL by 12 to provide a sufficient clock for USB
	rcc->CFGR |= (0b1010 << 18);

	// Enable SRAM
	rcc->AHBENR |= RCC_AHBENR_SRAMEN;

	// Enable DMA
	rcc->AHBENR |= RCC_AHBENR_DMA1EN;

	// Enable USART 1, IO A, ADC 1, ADC 2 (TODO: do we need the second one?)
	rcc->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

	// Enable USB
	rcc->APB1ENR |= RCC_APB1ENR_USBEN;

	uartUp();
}

uint32_t clockGetSysclkFrequency()
{
	return 48000000;
}

uint32_t clockGetApb2Prescaler()
{
	return 1U;
}
