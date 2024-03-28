//
// stm32_usb.h
//
// Created on: March 28, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#ifndef SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_
#define SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_

#include <stddef.h>
#include <stdint.h>

/// \brief API for interfacing w/ STM's USB BDT (RM0008 Rev 21 p 648)
/// \pre aOutBuffer is supposed to be able to accomodate `aReadSequenceLength`
/// half-words
/// \arg `aUsbBdtInnerOffset` -- inner offset in the buffer table descriptor
/// memory
/// \pre No fool protectione is used. The size of BDT must be accounted for
void usStm32f1UsbReadBdt(uint16_t *aOutBuffer, size_t aReadSequenceLength, size_t aUsbBdtInnerOffset);

/// \brief Writes `aInBuffer` into USB BDT memory
/// \pre No fool protectione is used. The size of BDT must be accounted for
/// \details Quite similar to `usStm32UsbReadBdt`. Refer to its description
void usStm32f1UsbWriteBdt(uint16_t *aInBuffer, size_t aWriteSequenceLength, size_t aUsbBdtInnerOffset);

#endif  // SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_
