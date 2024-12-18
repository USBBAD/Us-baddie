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
static void scaleBuffer(uint16_t *aBuffer, uint16_t aBufferSize); /**< Converts U12 (ADC) to I16 (USB mono) w/ scaling */

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint64_t sLastStatUpdateUs = 0;
static uint16_t *sDmaAudioBuffer;
static uint16_t sDmaBufferSize = 0;
static int sDebugPin2State = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void scaleBuffer(uint16_t *aBuffer, uint16_t aBufferSize)
{
	int16_t *buffer = (int16_t *)aBuffer;
	for (uint16_t i = 0; i < aBufferSize; ++i) {
		buffer[i] = (buffer[i] - 2048) * 8;
	}
}

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
	usDebugSetLed(1, 1);
	/* push second half of the buffer */
	scaleBuffer(sDmaAudioBuffer + sDmaBufferSize / 2, sDmaBufferSize / 2);
	usbMicrophonePushAudio(sDmaAudioBuffer + sDmaBufferSize / 2, sDmaBufferSize / 2);
}

static void onAdcDmaHalfTransferIsr(void)
{
	/* push the first half of the buffer */
	usDebugSetLed(1, 0);
	scaleBuffer(sDmaAudioBuffer, sDmaBufferSize / 2);
	usbMicrophonePushAudio(sDmaAudioBuffer, sDmaBufferSize / 2);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbAudioOnTransmitted()
{
	usDebugSetLed(2, !sDebugPin2State);
	sDebugPin2State = !sDebugPin2State;
}

void taskRunAudio(void)
{
	usDebugSetLed(1, 0);
	usDebugSetLed(2, sDebugPin2State);
	sDmaAudioBuffer = dmaGetBufferIsr(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, &sDmaBufferSize);
	usbMicrophoneInitAudio();
	dmaSetIsrHook(US_AUDIO_DMA_NUM, US_AUDIO_DMA_CHAN, onAdcDmaIsr, onAdcDmaHalfTransferIsr);
	uint64_t now = timeGetUptimeUs();
	adcStart();
	while (1) {
		now = timeGetUptimeUs();
		updateAudioStats();

#if 0
		usvprintf("Audio L, R, L, R,...");
		usDebugPrintU16Array(dmaBuffer, 16);
		usvprintf("\r\n");
#endif
		sysStatPrintPeriod(1000000);
	}
}

#endif /* SRC_APPLICATION_DEFAULT_DEBUG_ADC_USB_C_ */
