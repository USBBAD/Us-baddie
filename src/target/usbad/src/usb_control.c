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

struct UsbDeviceDescriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUsb;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} __attribute__((packed));

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void handleSetupBmRequestEndpoint(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize);
static void handleSetupBmRequestInterface(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize);
static void handleSetupBmRequestDevice(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize);
static void ep0OnRx(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *, const void *aBuffer, size_t aSize);

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct HalUsbDeviceDriver sEp0UsbDriver = {
	.priv = 0,
	.onRxIsr = ep0OnRx,
};

static struct UsbDeviceDescriptor sUsbDeviceDescriptor = {
	.bLength = 18,
	.bDescriptorType = 1,
	.bcdUsb = 0x0200, // TODO
	.bDeviceClass = 0x00, // TODO
	.bDeviceSubClass = 0x00, // TODO
	.bDeviceProtocol = 0x00, // TODO
	.bMaxPacketSize = 64,
	.idVendor = 0x0483, // TODO
	.idProduct = 0x5722, // TODO
	.bcdDevice = 1,
	.iManufacturer = 0,
	.iProduct = 0,
	.iSerialNumber = 0,
	.bNumConfigurations = 1,
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void handleSetupBmRequestEndpoint(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Endpoint request");
}

static void handleSetupBmRequestInterface(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Interface request");
}

static void handleSetupBmRequestDevice(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Device request");

	switch (aSetupTransaction->bRequest) {
		case UsbBRequestSetAddress:
			// TODO:
			break;
		case UsbBRequestGetDescriptor:
			halUsbDeviceWriteTxIsr(aDriver, 0, (const void *)&sUsbDeviceDescriptor, 18, 1);
			break;
		default:
			debugRegdumpEnqueueI32Context("Unhandled bRequest", aSetupTransaction->bRequest);

			break;
	}
}

static void ep0OnRx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext, const void *aBuffer,
	size_t aSize)
{
	switch (aContext->onRxIsr.transactionFlags & (HalUsbTransactionIn | HalUsbTransactionOut | HalUsbTransactionSetup)) {
		case HalUsbTransactionSetup: {
			usDebugPushMessage(getDebugToken(), "SETUP transaction");
			// Hanlde setup transaction
			struct SetupTransaction setupTransaction = *(const struct SetupTransaction *)aBuffer;
			switch (setupTransaction.bmRequestType & (UsbBmRequestTypeRecipientMask)) {
				case UsbBmRequestTypeRecipientEndpoint:
					handleSetupBmRequestEndpoint(aDriver, aContext, &setupTransaction, aBuffer, aSize);
					break;
				case UsbBmRequestTypeRecipientInterface:
					handleSetupBmRequestInterface(aDriver, aContext, &setupTransaction, aBuffer, aSize);
					break;
				case UsbBmRequestTypeRecipientDevice:
					handleSetupBmRequestDevice(aDriver, aContext, &setupTransaction, aBuffer, aSize);
					break;
			}
			break;
		}
		case HalUsbTransactionIn: {
			usDebugPushMessage(getDebugToken(), "IN transaction");
			break;
		}
		case HalUsbTransactionOut: {
			usDebugPushMessage(getDebugToken(), "OUT transaction");
			break;
		}
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

