//
// clock.h
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#ifndef TARGET_STM32F103C6_SRC_CLOCK_HPP_
#define TARGET_STM32F103C6_SRC_CLOCK_HPP_

/****************************************************************************
* Included Files
****************************************************************************/

#include <stdint.h>

/****************************************************************************
* Pre-processor Definitions
****************************************************************************/

/****************************************************************************
* Public Types
****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
* Public Data
****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
* Public Function Prototypes
****************************************************************************/

/// \brief Initializes clocking, adjusts frequency, enables peripherals
/// required for the application
void clockInitialize();

/// \brief Returns SYSCLK frequency (see RM0008, rev. 21, p.93)
uint32_t clockGetSysclkFrequency();

/// \brief Returns APB2's prescaler value
uint32_t clockGetApb2Prescaler();

/// \brief Returns APB2's prescaler value
uint32_t clockGetApb1Prescaler();

/// \brief Returns APB2's prescaler value
uint32_t clockGetAhbPrescaler();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline uint32_t getUsart1InputClockFrequency()
{
	return clockGetSysclkFrequency() / clockGetApb2Prescaler() / clockGetAhbPrescaler();
}

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif // TARGET_STM32F103C6_SRC_CLOCK_HPP_
