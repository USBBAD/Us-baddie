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

#include "application/default/target.h"
#include "clock.h"
#include "driver/usb_microphone/stub.h"
#include "driver/usb_microphone/usb_microphone.h"
#include "stm32f103c6_adc.h"
#include "stm32f103c6_dma.h"
#include "stm32f103c6_uart.h"
#include "target/arm/stm32f1/systick.h"
#include "usb.h"
#include "utility/ushelp.h"
#include <stm32f103x6.h>

extern void initializeGpio(void);
extern  void stm32f103c6DmaUpAdc(uint16_t *aDmaBuf, size_t aDmaBufSize, void(*aOnDmaFinishedHook)(),
	volatile ADC_TypeDef *aAdc);
extern  void stm32f103c6AdcUp(volatile ADC_TypeDef *aAdc, const uint8_t *aChannels, size_t aChannelsSize);

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void initializeBss(void);
static void memoryInitialize(void);
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

static void memoryInitialize(void)
{
	initializeBss();
	initializeData();
}


void targetInitialize(uint16_t *aAdcBuf, size_t aAdcBufSize, void(*aOnAdcCompleted)())
{
	static const uint8_t adcChannels[] = {1, 2}; /* TODO: Check if correct channels*/
	memoryInitialize();
	clockInitialize();
	systickInit(100);
	stm32f103c6AdcUp(ADC1, adcChannels, US_ARRAY_SIZE(adcChannels));
	stm32f103c6DmaUpAdc(aAdcBuf, aAdcBufSize, aOnAdcCompleted, ADC1);
	uartUp();
	initializeGpio();
	usbInitialize();
	usbMicrophoneInitUsbDriver();
	usbMicrophoneInitStub();
}
