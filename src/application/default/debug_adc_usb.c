/**
 * debug_adc_usb.c
 *
 * Created on: September 15, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_
#define SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "hal/adc.h"
#include "hal/dma.h"
#include "system/time.h"
#include "utility/debug.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint16_t audioBuffer[2] = {0};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void onAdcDmaIsr(void)
{
	usDebugPushMessage(0, "got DMA ISR");
	uint16_t *buffer = dmaGetBufferIsr(1, 1);
	audioBuffer[0] = buffer[0];
	audioBuffer[1] = buffer[1];
	adcStopIsr();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void taskRunAudio(void)
{
	dmaSetIsrHook(1, 1, onAdcDmaIsr);
	const uint64_t kMultisamplePeriodUs = 200000;
	uint64_t now = timeGetUptimeUs();
	uint64_t nextSampleTimestamp = now + kMultisamplePeriodUs;
	while (1) {
		now = timeGetUptimeUs();
		if (now > nextSampleTimestamp) {
			nextSampleTimestamp = now + kMultisamplePeriodUs;
			adcStart();
		} else {
			usvprintf("Audio L %u R %u\r\n", audioBuffer[0], audioBuffer[1]);
		}
	}
}

#endif /* SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_ */
