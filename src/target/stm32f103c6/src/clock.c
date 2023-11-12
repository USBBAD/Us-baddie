//
// clock.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include <stm32f103x6.h>
#include <stdint.h>

/// Whether HSI should be used as the clock source
#define USE_HSI_AS_CLOCK_SOURCE (1)

#ifndef USE_HSI_AS_CLOCK_SOURCE
#error "HSI should be used as the clock source"
#endif  /* USE_HSI_AS_CLOCK_SOURCE */

/// Internal clock source frequency
#define HSI_OUTPUT_FREQUENCY (8000000U)

#define USE_HSI_AS_SYSTEM_CLOCK (1)

#if USE_HSI_AS_SYSTEM_CLOCK
#define SYSCLK_OUTPUT_FREQUENCY (HSI_OUTPUT_FREQUENCY)
#else
#error "Unhandled clock configuration"
#endif

void clockInitialize()
{
	volatile RCC_TypeDef *rcc = RCC;
	// Enable Internal 8 MHz HSI clock source
	rcc->CR |= RCC_CR_HSION;

	// Wait until HSI is ready
	while (!(rcc->CR & RCC_CR_HSIRDY));

#if USE_HSI_AS_SYSTEM_CLOCK
	// Use HSI as the SYSCLK source
	rcc->CFGR |= RCC_CFGR_SW_HSI;
#endif

	// Enable SRAM
	rcc->AHBENR |= RCC_AHBENR_SRAMEN;
	// Enable USART 1, IO A, ADC 1, ADC 2 (TODO: do we need the second one?)
	rcc->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;
	// Enable USB
	rcc->APB1ENR |= RCC_APB1ENR_USBEN;
}

uint32_t clockGetSysclkFrequency()
{
#ifdef USE_HSI_AS_SYSTEM_CLOCK
	return HSI_OUTPUT_FREQUENCY;
#endif
}
