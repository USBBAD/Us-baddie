//
// clock.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define US_CLOCK_AHBPRE (2)
#define US_CLOCK_APB1PRE (2)
#define US_CLOCK_APB2PRE (2)
#define US_CLOCK_ADCPRE (8)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "stm32f103c6_uart.h"
#include <stm32f103x6.h>
#include <stdint.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void clockInitializeHse72Mhz(void);
static void clockInitializeHsi48Mhz();
static void clockInitializeHsi8Mhz();
static void enableAdc1Clock();
static void enableUsbClock();
static void enableGpioAClock();
static void enableDma1Clock();
static void enableUsart1Clock();

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint64_t sSysclk;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/** \details Necessary to get USB operating (stm32f103x6 datasheet, DocID15060
 * Rev 7)
 * RCC APB1 frequency = 72000000;
 * RCC APB2 frequency = 36000000;
 * RCC AHB frequency = 72000000;
 */
static void clockInitializeHse72Mhz(void)
{
	volatile RCC_TypeDef *rcc = RCC;
	volatile FLASH_TypeDef *flash = FLASH;

	// Enable HSE
	rcc->CR |= RCC_CR_HSEON;

	while (!(rcc->CR & RCC_CR_HSERDY));

	if ((rcc->CR & RCC_CR_HSERDY) == 0) {
		// clock failed to become ready
		// TODO notify and exit
		return;
	}

	// Enable Prefetch Buffer
	flash->ACR |= FLASH_ACR_PRFTBE;

	// Flash 2 wait state
	flash->ACR &= ~FLASH_ACR_LATENCY;
	flash->ACR |= FLASH_ACR_LATENCY_2;

	/* HCLK = SYSCLK / AHBPRE */
#if US_CLOCK_AHBPRE == 1
	rcc->CFGR |= RCC_CFGR_HPRE_DIV1;
#elif US_CLOCK_AHBPRE == 2
	rcc->CFGR |= RCC_CFGR_HPRE_DIV2;
#else
#  error "Missing clock configuration for AHBPRE"
#endif /* US_CLOCK_AHBPRE */

	/* PCLK2 = HCLK / US_CLOCK_APB2PRE */
#if US_CLOCK_APB2PRE == 1
	rcc->CFGR |= RCC_CFGR_PPRE2_DIV1;
#elif US_CLOCK_APB2PRE == 2
	rcc->CFGR |= RCC_CFGR_PPRE2_DIV2;
#elif US_CLOCK_APB2PRE == 4
	rcc->CFGR |= RCC_CFGR_PPRE2_DIV4;
#else
#  error "Missing clock configuration for APBPRE2"
#endif

#if US_CLOCK_ADCPRE == 8
	rcc->CFGR |= RCC_CFGR_ADCPRE_DIV8;
#else
#  error "Missing clock configuration for ADCPRE"
#endif

	// PCLK1 = HCLK/2
#if US_CLOCK_APB1PRE == 2
	rcc->CFGR |= RCC_CFGR_PPRE1_DIV2;
#else
#  error "Missing clock configuration for APB1PRE"
#endif

	/* ADC prescaler = HCLK / US_CLOCK_ADCPRE */

	//  PLL configuration: PLLCLK = HSE * 9 = 72 MHz
	rcc->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
	rcc->CFGR |= (0x00010000 /* HSE */ | RCC_CFGR_PLLMULL9);

	// Enable PLL
	rcc->CR |= RCC_CR_PLLON;

	// Wait till PLL is ready
	while ((rcc->CR & RCC_CR_PLLRDY) == 0) {
		__asm volatile("nop");
	}

	// Select PLL as system clock source
	rcc->CFGR &= ~RCC_CFGR_SW;
	rcc->CFGR |= RCC_CFGR_SW_PLL;

	// Wait till PLL is used as system clock source
	while ((rcc->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
		__asm volatile("nop");
	}

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
	rcc->CFGR |= RCC_CFGR_PLLMULL12;

	sSysclk = 48000000;
}

static inline void enableDma1Clock()
{
	volatile RCC_TypeDef *rcc = RCC;

	// Enable DMA
	rcc->AHBENR |= RCC_AHBENR_DMA1EN;
}

static inline void enableGpioAClock()
{
	volatile RCC_TypeDef *rcc = RCC;

	rcc->APB2ENR |= RCC_APB2ENR_IOPAEN;
}

static inline void enableAdc1Clock()
{
	volatile RCC_TypeDef *rcc = RCC;

	// Enable DMA
	rcc->APB2ENR |=  RCC_APB2ENR_ADC1EN;
}

static inline void enableUsbClock()
{
	volatile RCC_TypeDef *rcc = RCC;

	// Enable DMA
	rcc->APB1ENR |= RCC_APB1ENR_USBEN;
}

static void enableUsart1Clock()
{
	volatile RCC_TypeDef *rcc = RCC;

	rcc->APB2ENR |= RCC_APB2ENR_USART1EN;
}

void clockInitializeHsi8Mhz()
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

	sSysclk = 8000000;
}

void clockInitialize()
{
	// Initialize SYSCLK to 72 MHz
	clockInitializeHse72Mhz();

	// Enable peripherals' clock
	enableDma1Clock();
	enableGpioAClock();
	enableAdc1Clock();
	enableUsbClock();
	enableUsart1Clock();
}

uint32_t clockGetSysclkFrequency()
{
	return sSysclk;
}

uint32_t clockGetAhbPrescaler()
{
	return US_CLOCK_AHBPRE;
}

uint32_t clockGetApb1Prescaler()
{
	return US_CLOCK_APB1PRE;
}

uint32_t clockGetApb2Prescaler()
{
	return US_CLOCK_APB2PRE;
}
