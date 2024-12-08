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

#include "driver/usb_microphone/audio.h"
#include "hal/adc.h"
#include "hal/dma.h"
#include "system/stat.h"
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
	uint16_t dmaBufSz;
	const uint16_t *dmaBuffer = dmaGetBufferIsr(1, 1, &dmaBufSz);
	usbMicrophoneInitAudio(dmaBuffer, dmaBufSz);
	dmaSetIsrHook(1, 1, onAdcDmaIsr);
	const uint64_t kMultisamplePeriodUs = 200000;
	uint64_t now = timeGetUptimeUs();
	uint64_t nextSampleTimestamp = now + kMultisamplePeriodUs;
	while (1) {
		now = timeGetUptimeUs();
		adcStart();
		timeBusywaitUs(2000000);

#if 0
		usvprintf("Audio L, R, L, R,...");
		usDebugPrintU16Array(dmaBuffer, 16);
		usvprintf("\r\n");
#endif
		sysStatPrint();
//		usDebugIterDebugLoop();
	}
}

#endif /* SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_ */
