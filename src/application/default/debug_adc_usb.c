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
//	usDebugPushMessage(0, "got DMA ISR");
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
		adcStart();
		timeBusywaitUs(200000);

		const uint16_t *dmaBuffer = dmaGetBufferIsr(1, 1);
		usvprintf("Audio L, R, L, R,...");
		usDebugPrintU16Array(dmaBuffer, 16);
		usvprintf("\r\n");
	}
}

#endif /* SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_ */
