/**
 * usb_microphone.c
 *
 * Created on: July 31, 2024
 *     Author: Dmitry Murashov
 */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/usb_microphone.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct UsbMicrophoneLogicState {
	/**
	 * @brief enabled. Set, when AS Setting 1 is activated (Interface 1
	 * is set)
	 */
	int enabled;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct UsbMicrophoneLogicState sLogicState = {
	.enabled = 0,
};

struct UsbMicrophoneHook *gUsbMicrophoneHook;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbMicrophoneSetEnabled(int aEnabled)
{
	sLogicState.enabled = aEnabled;
	if (gUsbMicrophoneHook) {
		gUsbMicrophoneHook->onEnabledStateChangedIsr(aEnabled);
	}
}

int usbMicrophoneIsEnabled()
{
	return sLogicState.enabled;
}

void usbMicrophoneSetHook(struct UsbMicrophoneHook *aHook)
{
	gUsbMicrophoneHook = aHook;
}
