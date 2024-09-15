/**
 * audio.c
 *
 * Created on: September 15, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_DRIVER_USB_MICROPHONE_AUDIO_C_
#define SRC_DRIVER_USB_MICROPHONE_AUDIO_C_

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/usb_microphone.h"
#include <stddef.h>
#include <stdint.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void onEnabledStateChangedIsr(int aEnabled);
static void onChunkTransmitted();

/****************************************************************************
 * Private Data
 ****************************************************************************/

/** \struct UsbMicrophoneHook is a high-level driver that handles buffer management
 * during audio transmission
 */
static struct UsbMicrophoneHook sUsbMicrophoneHook = {
	.onChunkTransmitted = onChunkTransmitted,
	.onEnabledStateChangedIsr = onEnabledStateChangedIsr,
};

static const uint16_t *sMonoPcmBuf; /**< A pointer to external buffer */
static size_t sMonoPcmBufSize;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void onEnabledStateChangedIsr(int aEnabled)
{
	if (aEnabled) {
		usbMicrophoneSetMonoPcm16Buffer(&sMonoPcmBuf[0], sMonoPcmBufSize);
	}
}

void onChunkTransmitted()
{
	if (usbMicrophoneIsEnabled()) {
		usbMicrophoneSetMonoPcm16Buffer(&sMonoPcmBuf[0], sMonoPcmBufSize);
	}
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbMicrophoneInitAudio(const uint16_t *aMonoPcmBuf, size_t aBufSize)
{
	sMonoPcmBuf = aMonoPcmBuf;
	sMonoPcmBufSize = aBufSize;
	usbMicrophoneSetHook(&sUsbMicrophoneHook);
}

#endif /* SRC_DRIVER_USB_MICROPHONE_AUDIO_C_ */

