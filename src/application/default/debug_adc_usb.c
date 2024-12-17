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
static void onAdcDmaIsr(void);
static void onAdcDmaHalfTransferIsr(void);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint64_t sLastStatUpdateUs = 0;
static uint16_t *sDmaAudioBuffer;
static uint16_t sDmaBufferSize = 0;

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
	/* push second half of the buffer */
	usbMicrophonePushAudio(sDmaAudioBuffer + sDmaBufferSize / 2, sDmaBufferSize / 2);
}

static void onAdcDmaHalfTransferIsr(void)
{
	/* push the first half of the buffer */
	usbMicrophonePushAudio(sDmaAudioBuffer, sDmaBufferSize / 2);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbAudioOnTransmitted()
{
}

void taskRunAudio(void)
{
	sDmaAudioBuffer = dmaGetBufferIsr(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, &sDmaBufferSize);
	usbMicrophoneInitAudio();
	dmaSetIsrHook(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, onAdcDmaIsr, onAdcDmaHalfTransferIsr);
	uint64_t now = timeGetUptimeUs();
	adcStart();
	while (1) {
		now = timeGetUptimeUs();
//		timeBusywaitUs(20);
		updateAudioStats();

#if 0
		usvprintf("Audio L, R, L, R,...");
		usDebugPrintU16Array(dmaBuffer, 16);
		usvprintf("\r\n");
#endif
		sysStatPrintPeriod(1000000);
//		usDebugIterDebugLoop();
	}
}

#endif /* SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_ */
