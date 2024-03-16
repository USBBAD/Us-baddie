//
// clock.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include "stm32f103c6_uart.h"
#include <stm32f103x6.h>
#include <stdint.h>

static void clockInitializeHse72Mhz();
static void clockInitializeHsi48Mhz();

static uint64_t sSysclk;

// \details Necessary to get USB operating (stm32f103x6 datasheet, DocID15060
// Rev 7)
// RCC APB1 frequency = 72000000;
// RCC APB2 frequency = 36000000;
// RCC AHB frequency = 72000000;
static void clockInitializeHse72Mhz()
{
	volatile RCC_TypeDef *rcc = RCC;
	// Enable Internal 8 MHz HSI clock source
	rcc->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	while (!(rcc->CR & RCC_CR_HSERDY));

	// APB 1, 36 MHz max, should not exceed, div. SYSCLK
	rcc->CFGR |= RCC_CFGR_PPRE1_DIV2;

	// Enable 1.5 prescaler to get 48MHz on USB bus
	rcc->CFGR &= ~RCC_CFGR_USBPRE;

	{
		// High clock requires setting latency on flash memory access, RM0008 rev. 21
		volatile FLASH_TypeDef *flash = FLASH;
		uint32_t ws = 2;
		// TODO: iffy, as it was imported from CDCACM code
		volatile uint32_t reg32 = flash->ACR;
		reg32 &= ~(FLASH_ACR_LATENCY_Msk << FLASH_ACR_LATENCY_Pos);
		reg32 |= (ws << FLASH_ACR_LATENCY_Pos);
		flash->ACR = reg32;
	}

	// Set PLLMUL x 9
	rcc->CFGR |= RCC_CFGR_PLLMULL9;  // TODO iffy

	// Select external as PLL clock source
	rcc->CFGR |= RCC_CFGR_PLLSRC;

	// Enable PLL
	rcc->CR |= RCC_CR_PLLON;

	// Wait for PLL to get ready
	while (!(rcc->CR & RCC_CR_PLLRDY));

	// Use PLL as the SYSCLK source (8 MHz external oscillator)
	rcc->CFGR |= RCC_CFGR_SW_PLL;

	sSysclk = 72000000;
}

/// \brief 48 MHz at SYSCLK, HSI as system clock, USB-compatible
static void clockInitializeHsi48Mhz()
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
	sSysclk = 48000000;
}

void clockInitialize()
{
	clockInitializeHse72Mhz();
}

uint32_t clockGetSysclkFrequency()
{
	return 48000000;
}

uint32_t clockGetApb2Prescaler()
{
	return 1U;
}
