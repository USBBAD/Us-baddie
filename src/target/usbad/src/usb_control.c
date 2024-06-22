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
static void ep0OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext);

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct HalUsbDeviceDriver sEp0UsbDriver = {
	.priv = 0,
	.onRxIsr = ep0OnRx,
	.onTxIsr = ep0OnTx,
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

static uint8_t sConfigDescriptor[] = {
    // Config 0 header
    9,                                //  Length
    0x02,                             //  CONFIGURATION Descriptor Type
    9 + 9 + 7 + 7, 0,                 //  TotalLength
    1,                                //  NumInterfaces
    1,                                //  ConfigurationValue
    0,                                //  Configuration string not set
    0x80,                             //  Attributes 0x80 for historical reasons
    50,                               //  MaxPower 100mA

    // interface 0
    9,    // Length
    0x04, // INTERFACE Descriptor Type
    0, 0, // Interface Number, Alternate Setting
    2,    // Num Endpoints
    0x0A, // InterfaceClass: USB_CLASS_DATA
    0,    // InterfaceSubClass
    0,    // InterfaceProtocol
    0,    // Interface string not set

    // endpoint 0x1
    7,     //  Length
    0x05,  //  ENDPOINT Descriptor Type
    0x01,  //  Endpoint Address: 1-OUT
    0x02,  //  Attributes: BULK
    64, 0, //  MaxPacketSize
    0,     //  Interval, ignored for BULK

    // endpoint 0x81
    7,     //  Length
    0x05,  //  ENDPOINT Descriptor Type
    0x81,  //  Endpoint Address 1-IN
    0x02,  //  Attributes: BULK
    64, 0, //  MaxPacketSize
    0,     //  Interval, ignored for BULK
};

struct {
	int16_t address;
} sDriverState = {-1};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline void handleSetupBmRequestEndpoint(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Endpoint request");
}

static inline void handleSetupBmRequestInterface(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Interface request");
	switch (aSetupTransaction->bRequest) {
		case UsbBRequestGetDescriptor: {
			usDebugPushMessage(getDebugToken(), "GET_DESCRIPTOR intefrace");
		}
		default:
			break;
	}
}

static inline void handleSetupBmRequestDevice(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Device request");
	switch (aSetupTransaction->bRequest) {
		case UsbBRequestSetAddress:
			sDriverState.address = aSetupTransaction->wValue;
			// To finish status transaction
			halUsbDeviceWriteTxIsr(aDriver, 0, "", 0, 1);
			break;
		case UsbBRequestGetDescriptor:
			usDebugPushMessage(getDebugToken(), "GET_DESCRIPTOR");
			switch (aSetupTransaction->wValue) {
				case 0x0100:
					halUsbDeviceWriteTxIsr(aDriver, 0, (const void *)&sUsbDeviceDescriptor, 18, 1);
					break;
				case 0x0200:
					halUsbDeviceWriteTxIsr(aDriver, 0, (const void *)&sConfigDescriptor[0], aSetupTransaction->wLength, 1);
			}
			break;
		case UsbBRequestGetConfiguration:
			usDebugPushMessage(getDebugToken(), "GET_CONFIGURATION");
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
			usDebugPushMessage(getDebugToken(), "IN transaction. Should have never got here!");
			break;
		}
		case HalUsbTransactionOut: {
			usDebugPushMessage(getDebugToken(), "OUT transaction");
			break;
		}
	}
}

static void ep0OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext)
{
	switch (aContext->onRxIsr.transactionFlags & (HalUsbTransactionIn | HalUsbTransactionOut | HalUsbTransactionSetup)) {
		case HalUsbTransactionIn: {
			usDebugPushMessage(getDebugToken(), "IN transaction");
			if (sDriverState.address >= 0) {
				// Status transaction has been finished, transfer is finalized, now set the device's address
				halUsbDeviceSetAddress(aDriver, (uint8_t)sDriverState.address);
				sDriverState.address = -1;
			}
			break;
		}
		default:
			break;
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

