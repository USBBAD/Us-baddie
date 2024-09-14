/**
 * systick.c
 *
 * Created on: September 14, 2024
 *     Author: Dmitry Murashov
 */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "clock.h"
#include <stm32f103x6.h>
#include <stdint.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef uint64_t TimeType;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct {
	TimeType resolutionUs;
	TimeType uptimeUs;
} sSystick = {0U, 0U};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

uint64_t timeGetUptimeUs()
{
	return (uint64_t)sSystick.uptimeUs;
}

void SysTick_Handler(void)
{
	sSystick.uptimeUs += sSystick.resolutionUs;
}

void systickInit(uint32_t aResolutionUs)
{
	const uint32_t ahbFreq = clockGetSysclkFrequency() / clockGetAhbPrescaler();
	const uint32_t reloadValue = ahbFreq / 8 * aResolutionUs / 1000000;
	sSystick.resolutionUs = (TimeType)aResolutionUs;
	SysTick->LOAD = reloadValue;
	SysTick->CTRL = (1 << SysTick_CTRL_ENABLE_Pos)
		| (0 << SysTick_CTRL_CLKSOURCE_Pos) //
		| (1 << SysTick_CTRL_TICKINT_Pos);
	NVIC_EnableIRQ(SysTick_IRQn);
}
