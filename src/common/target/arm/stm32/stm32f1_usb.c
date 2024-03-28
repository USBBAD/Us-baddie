//
// stm32f1_usb.h
//
// Created on: March 28, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#ifndef SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_
#define SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_

#define US_STM32F1_BDT_TABLE_AHB_ADDRESS (0x40006000)

#include <stddef.h>
#include <stdint.h>

static inline uint32_t *bdtOffset2AhbOffset(size_t aUsbBdtInnerOffset)
{
	return (uint32_t *)(US_STM32F1_BDT_TABLE_AHB_ADDRESS + (aUsbBdtInnerOffset * 2));
}

void usStm32f1UsbReadBdt(uint16_t *aOutBuffer, size_t aReadSequenceLength, size_t aUsbBdtInnerOffset)
{
	const uint32_t *currentOffset = bdtOffset2AhbOffset(aUsbBdtInnerOffset);

	for (; aReadSequenceLength; --aReadSequenceLength) {
		*aOutBuffer = (uint16_t)(*currentOffset);
		++currentOffset;
		++aOutBuffer;
	}
}

void usStm32f1UsbWriteBdt(uint16_t *aInBuffer, size_t aWriteSequenceLength, size_t aUsbBdtInnerOffset)
{
	uint32_t *currentOffset = bdtOffset2AhbOffset(aUsbBdtInnerOffset);

	for (; aWriteSequenceLength; --aWriteSequenceLength) {
		*currentOffset = *aInBuffer;
		++aInBuffer;
		++currentOffset;
	}
}

#endif  // SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_

