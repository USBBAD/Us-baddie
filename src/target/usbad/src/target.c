//
// target.c
//
// Created: 2023-10-13
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "clock.h"
#include "driver/usb_microphone/audio.h"
#include "driver/usb_microphone/stub.h"
#include "driver/usb_microphone/usb_microphone.h"
#include "stm32f103c6_adc.h"
#include "stm32f103c6_dma.h"
#include "stm32f103c6_uart.h"
#include "target/arm/stm32f1/systick.h"
#include "target/target.h"
#include "usb.h"

extern void initializeGpio(void);

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void initializeBss(void);
static void initializeData(void);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/// @brief Sets the entire .bss section w/ NULL
static void initializeBss(void)
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
static void initializeData(void)
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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void memoryInitialize(void)
{
	initializeBss();
	initializeData();
}

void targetInitialize(void)
{
	clockInitialize();
	systickInit(100);
	stm32f103c6DmaUp();
	stm32f103c6AdcUp();
	uartUp();
	initializeGpio();
	usbInitialize();
	usbMicrophoneInitUsbDriver();
}
