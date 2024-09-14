//
// target.h
//
// Created on: 2023-10-11
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL>)
//

#ifndef USBAD_SRC_COMMON_TARGET_TARGET_H_
#define USBAD_SRC_COMMON_TARGET_TARGET_H_

#include "hal/adc.h"

/// \brief Performs all the target-dependent initialization
/// \returns Target handle, if successful. 0 otherwise
void targetInitialize();
void memoryInitialize();

#endif  // USBAD_SRC_COMMON_TARGET_TARGET_H_
