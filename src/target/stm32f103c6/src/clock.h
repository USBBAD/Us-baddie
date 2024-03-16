//
// clock.h
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include <stdint.h>

#ifndef TARGET_STM32F103C6_SRC_CLOCK_HPP_
#define TARGET_STM32F103C6_SRC_CLOCK_HPP_

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

#endif // TARGET_STM32F103C6_SRC_CLOCK_HPP_
