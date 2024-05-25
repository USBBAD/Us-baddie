//
// usb_control.c
//
// Created on: May 19, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_USBAD_SRC_USB_CONTROL_C_
#define SRC_TARGET_USBAD_SRC_USB_CONTROL_C_

#include "hal/usb.h"
#include "utility/debug.h"

#define USBAD_DEBUG_REGDUMP_FIFO_SIZE (4)
#include "utility/debug_regdump.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct SetupTransaction {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} __attribute__((packed));

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

void ep0OnRx(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *, const void *aBuffer, size_t aSize);

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct HalUsbDeviceDriver sEp0UsbDriver = {
	.priv = 0,
	.onRxIsr = ep0OnRx,
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void ep0OnRx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext, const void *aBuffer,
	size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Holy fuck!");

	// Hanlde setup transaction
	if (aContext->onRxIsr.transactionFlags & (HalUsbTransactionSetup)) {
		// Unpack
		struct SetupTransaction setupTransaction = *(struct SetupTransaction const *)aBuffer;
	}
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void initializeEp0UsbHalDeviceDriver()
{
	halUsbDeviceRegisterDriver(&sEp0UsbDriver, 0);
	usDebugPushMessage(getDebugToken(), "Initialization completed");
	debugRegdumpInitialize("usbctl");
}

#endif  // SRC_TARGET_USBAD_SRC_USB_CONTROL_C_

