//
// clock.h
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#ifndef TARGET_STM32F103C6_SRC_CLOCK_HPP_
#define TARGET_STM32F103C6_SRC_CLOCK_HPP_

/// \brief Initializes clocking, adjusts frequency, enables peripherals
/// required for the application
void clockInitialize();

#endif // TARGET_STM32F103C6_SRC_CLOCK_HPP_
