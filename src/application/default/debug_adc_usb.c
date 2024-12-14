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

#define US_AUDIO_STAT_PERIOD_UPDATE_US (2000000)
#define US_AUDIO_DMA_NUM (1)
#define US_AUDIO_DMA_CHAN (1)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/audio.h"
#include "hal/adc.h"
#include "hal/dma.h"
#include "system/stat.h"
#include "system/time.h"
#include "utility/debug.h"
#include "utility/ushelp.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void updateAudioStats();

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint64_t sLastStatUpdateUs = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void updateAudioStats()
{
	// Check timeout
	const uint64_t now = timeGetUptimeUs();
	if (now - sLastStatUpdateUs > US_AUDIO_STAT_PERIOD_UPDATE_US) {
		sLastStatUpdateUs = now;
	} else {
		return;
	}

	// Get the mean
	{
		uint16_t dmaBufSz;
		const uint16_t *dmaBuf = dmaGetBufferIsr(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, &dmaBufSz);
		gSysStat.audioMean = (uint16_t)(usSumU16(dmaBuf, dmaBufSz) / (uint16_t)dmaBufSz);
		gSysStat.audioAmplitude = (uint16_t)(usMaxU16(dmaBuf, dmaBufSz) - usMinU16(dmaBuf, dmaBufSz));
	}
}

static void onAdcDmaIsr(void)
{
	adcStopIsr();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void taskRunAudio(void)
{
	uint16_t dmaBufSz;
	const uint16_t *dmaBuffer = dmaGetBufferIsr(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, &dmaBufSz);
	usbMicrophoneInitAudio(dmaBuffer, dmaBufSz);
	dmaSetIsrHook(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, onAdcDmaIsr);
	uint64_t now = timeGetUptimeUs();
	while (1) {
		now = timeGetUptimeUs();
		adcStart();
		timeBusywaitUs(2000000);
		updateAudioStats();

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
