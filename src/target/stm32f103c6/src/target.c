//
// target.c
//
// Created: 2023-10-13
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

#include "clock.h"
#include "stm32f103c6_adc.h"
#include "stm32f103c6_dma.h"
#include "stm32f103c6_uart.h"
#include "usb.h"
#include "target/target.h"

struct Target
{
};

static struct Target sTarget;

static void initializeBss();
static void initializeData();

/// @brief Sets the entire .bss section w/ NULL
static void initializeBss()
{
	// See the linker script where those symbols are defined
	extern unsigned long int gBssStart;
	extern unsigned long int gBssEnd;

	for (volatile register unsigned long int *bssIt = &gBssStart; bssIt < &gBssEnd; ++bssIt)
	{
		*bssIt = 0;
	}
}

/// @brief Copies default values for .data section from flash memory
static void initializeData()
{
	// See the linker script where those symbols are defined
	extern int gRamDataStart;
	extern int gRamDataEnd;
	extern int gFlashDataStart;

	for (volatile register int *flashDataIt = &gFlashDataStart, *ramDataIt = &gRamDataStart;
		ramDataIt < &gRamDataEnd;
		++flashDataIt, ++ramDataIt
	) {
		*ramDataIt = *flashDataIt;
	}
}

TargetHandle targetInitialize()
{
	initializeBss();
	initializeData();
	clockInitialize();
	stm32f103c6DmaUp();
	stm32f103c6AdcUp();
	uartUp();
	usbInitialize();

	return (TargetHandle)&sTarget;
}
