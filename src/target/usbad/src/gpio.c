/**
 * gpio.c
 *
 * Created on: August 18, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_TARGET_USBAD_SRC_GPIO_C_
#define SRC_TARGET_USBAD_SRC_GPIO_C_

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include <stm32f103x6.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

enum Gpio {
	GpioPc13 = 0,
	GpioPa6 = 1,
	GpioPa7 = 2,
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void initializeGpio(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	/* Initialize PC13 as open-drain */
	GPIOC->CRH = 0b01 << GPIO_CRH_CNF13_Pos
		| 0b11 << GPIO_CRH_MODE13_Pos;

	/* Initialize debug pins: PA6, PA7. Output, push-pull */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA-> CRL |= 0b11 << GPIO_CRL_MODE7_Pos
		| 0b11 << GPIO_CRL_MODE6_Pos;
}

/**
 * @brief usDebugSetLedf sets LEDs (on Blue Pill, only PC13 is available)
 * @param aLed number of led, counting from 0
 * @param aState 1 for on, 0 for off
 */
void usDebugSetLed(int aLed, int aState)
{
	switch (aLed) {
		case GpioPc13:
			GPIOC->BSRR |= aState ? (1 << GPIO_BSRR_BR13_Pos) : (1 << GPIO_BSRR_BS13_Pos);
			break;

		case GpioPa6:
			GPIOA->BSRR |= aState ? (1 << GPIO_BSRR_BR6_Pos) : (1 << GPIO_BSRR_BS6_Pos);
			break;

		case GpioPa7:
			GPIOA->BSRR |= aState ? (1 << GPIO_BSRR_BR7_Pos) : (1 << GPIO_BSRR_BS7_Pos);
			break;
	}
}

#endif /* SRC_TARGET_USBAD_SRC_GPIO_C_ */
