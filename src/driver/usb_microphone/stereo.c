//
// stereo.c
//
// Created on: September 01, 2024
//     Author: Dmitry Murashov
//

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define USBAD_DEBUG_REGDUMP_FIFO_SIZE (10)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/stereo.h"
#include "driver/usb_microphone/usb_microphone.h"
#include "utility/debug_regdump.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void onEnabledStateChangedIsrHook(int aEnabled);
static void onChunkTransmittedHook();

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct UsbMicrophoneHook sHook = {
	.onEnabledStateChangedIsr = onEnabledStateChangedIsrHook,
	.onChunkTransmitted = onChunkTransmittedHook,
};

static struct UsbMicrophoneStereo *sStereo = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void onEnabledStateChangedIsrHook(int aEnabled)
{
	/* TODO: make dependent on time synchronization */
	if (aEnabled) {
		usbMicrophoneSetStereoPcm16Buffer(sStereo->buffer, sStereo->size);
	}
}

static void onChunkTransmittedHook()
{
	/* TODO */
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbMicrophoneInitStereo(struct UsbMicrophoneStereo *aStereo)
{
	sStereo = aStereo;
	usbMicrophoneSetHook(&sHook);
}
