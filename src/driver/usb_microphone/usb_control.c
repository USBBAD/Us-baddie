//
// usb_control.c
//
// Created on: May 19, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_USBAD_SRC_USB_CONTROL_C_
#define SRC_TARGET_USBAD_SRC_USB_CONTROL_C_

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define USBAD_DEBUG_REGDUMP_FIFO_SIZE (10)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/usb_microphone.h"
#include "hal/usb.h"
#include "utility/debug.h"
#include "utility/ushelp.h"
#include "utility/debug_regdump.h"

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
	.bLength = 0x12,
	.bDescriptorType = 1, // Device descriptor
	.bcdUsb = 0x0200, // TODO: I use an example from USB Audio specs, and it uses USB 1.0 The 1.0 is also limited to 8 byte packages. However, the spec recommends adhering to USB 2.0
	.bDeviceClass = 0x00, // Defined at interface level
	.bDeviceSubClass = 0x00, // Defined at interface level
	.bDeviceProtocol = 0x00, // Defined at interface level
	.bMaxPacketSize = 64,
	.idVendor = 0x0483, // TODO
	.idProduct = 0x5722, // TODO
	.bcdDevice = 1, // 0.01, beta version
	.iManufacturer = 1, // String #1 contains the manufacturer's name
	.iProduct = 2, // String #2 contains the prosduct's title
	.iSerialNumber = 0,
	.bNumConfigurations = 1, // One configuration
};

extern uint8_t config_descriptor[];
extern uint8_t *string_descriptor[];

struct {
	int16_t address; /**< Pending device address setting */
	uint8_t *sendBuffer; /**< Pending chunk-by-chunk sending */
	uint8_t *sendBufferEnd; /**< End position for chunk-by-chunk sending */
} sDriverState = {
	.address = -1,
	.sendBuffer = 0,
	.sendBufferEnd = 0,
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline void handleSetupBmRequestEndpoint(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	usDebugPushMessage(getDebugToken(), "Unhandled endpoint request");
}

static inline void handleSetupBmRequestInterface(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	switch (aSetupTransaction->bRequest) {
		case UsbBRequestSetInterface: {
			const uint16_t setInterfaceIndex = aSetupTransaction->wIndex;
			/* Which interface's configuration to choose. 0 - no audio data, 1 - stream 16-bit PCM */
			const uint16_t setInterfaceValue = aSetupTransaction->wValue;
			/* TODO: handle interface setting logic */

			if (setInterfaceValue == 0) {
				usbMicrophoneSetEnabled(0);
				usDebugPushMessage(getDebugToken(), "Disabled Audio");
			} else if (setInterfaceValue == 1) {
				usbMicrophoneSetEnabled(1);
				usDebugPushMessage(getDebugToken(), "Enabled Audio");
			} else {
				debugRegdumpEnqueueI32Context("Incorrect interface index:", setInterfaceValue);
			}

			/* Respond w/ ZLP */
			halUsbDeviceWriteTxIsr(aDriver, 0, 0, 0, !(aContext->onRxIsr.transactionFlags & HalUsbTransactionData1));
			break;
		}
		default:
			debugRegdumpEnqueueI32Context("Unhandled interface request", aSetupTransaction->bRequest);
			break;
	}
}

static inline void handleSetupBmRequestDevice(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext,
	const struct SetupTransaction *aSetupTransaction, const void *aBuffer, size_t aSize)
{
	switch (aSetupTransaction->bRequest) {
		case UsbBRequestGetStatus: {
			usDebugPushMessage(0, "![usb] GET STATUS unsupprted");
			/*
			static const uint8_t status[] = {0, 0};
			halUsbDeviceWriteTxIsr(aDriver, 0, status, US_ARRAY_SIZE(status),
				!(aContext->onRxIsr.transactionFlags & HalUsbTransactionData1));
			*/
			break;
		}
		case UsbBRequestSetAddress:
			sDriverState.address = aSetupTransaction->wValue;
			// To finish status transaction
			halUsbDeviceWriteTxIsr(aDriver, 0, "", 0, 1);
			break;
		case UsbBRequestSetConfiguration:
			usDebugPushMessage(getDebugToken(), "SET_CONFIG");
			halUsbDeviceWriteTxIsr(aDriver, 0, 0, 0, !(aContext->onRxIsr.transactionFlags & HalUsbTransactionData1));
			break;
		case UsbBRequestGetDescriptor: {
			const uint16_t descriptorType = (aSetupTransaction->wValue & 0xFF00) >> 8;
			const uint16_t descriptorIndex = aSetupTransaction->wValue & 0xFF;
#if 0
			debugRegdumpEnqueueI32Context("get descr. type", descriptorType);
#endif /* 0 */
			switch (descriptorType) {
				case 1: {
					size_t descriptorLength = 18;
					if (aSetupTransaction->wLength < descriptorLength) {
						descriptorLength = aSetupTransaction->wLength;
					}
					halUsbDeviceWriteTxIsr(aDriver, 0, (const void *)&sUsbDeviceDescriptor, descriptorLength,
						!(aContext->onRxIsr.transactionFlags & HalUsbTransactionData1));
					break;
				}
				case 2: {
					uint16_t descriptorLength = 100; // TODO: replace the magic number
					if (descriptorLength > aSetupTransaction->wLength) {
						descriptorLength = aSetupTransaction->wLength;
					}
					if (descriptorLength > sUsbDeviceDescriptor.bMaxPacketSize) {
						sDriverState.sendBuffer = &config_descriptor[sUsbDeviceDescriptor.bMaxPacketSize];
						sDriverState.sendBufferEnd = &config_descriptor[descriptorLength];
						descriptorLength = sUsbDeviceDescriptor.bMaxPacketSize;
					}
					halUsbDeviceWriteTxIsr(aDriver, 0, (const void *)&config_descriptor[0], descriptorLength,
						!(aContext->onRxIsr.transactionFlags & HalUsbTransactionData1));
					break;
				}
				case 3: {
					const size_t descriptorSize = string_descriptor[descriptorIndex][0];
					const void *descriptor = (const void *)&string_descriptor[descriptorIndex][0];
					halUsbDeviceWriteTxIsr(aDriver, 0, descriptor, descriptorSize,
						!(aContext->onRxIsr.transactionFlags & HalUsbTransactionData1));
					break;
				default:
					break;
				}
			}
			break;
		}
		default:
			debugRegdumpEnqueueI32Context("Unhandled device request", aSetupTransaction->bRequest);
			break;
	}
}

static void ep0OnRx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext, const void *aBuffer,
	size_t aSize)
{
	switch (aContext->onRxIsr.transactionFlags & (HalUsbTransactionIn | HalUsbTransactionOut | HalUsbTransactionSetup)) {
		case HalUsbTransactionSetup: {
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
			break;
		}
	}
}

static void ep0OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext)
{
	switch (aContext->onRxIsr.transactionFlags & (HalUsbTransactionIn | HalUsbTransactionOut | HalUsbTransactionSetup)) {
		case HalUsbTransactionIn: {
			if (sDriverState.address >= 0) {
				// Status transaction has been finished, transfer is finalized, now set the device's address
				halUsbDeviceSetAddress(aDriver, (uint8_t)sDriverState.address);
				sDriverState.address = -1;
			}
			if (sDriverState.sendBuffer != 0) {
				const size_t remaining = sDriverState.sendBufferEnd - sDriverState.sendBuffer;
				if (remaining == 0U) {
					// Reset driver state
					sDriverState.sendBuffer = 0;
					sDriverState.sendBufferEnd = 0;

					// Send ZLP
					halUsbDeviceWriteTxIsr(aDriver, 0, 0, 0,
						!(aContext->onTxIsr.transactionFlags & HalUsbTransactionData1)); // TODO: DATA0?
				} else {
					// Send as much as permitted by the device descriptor
					const size_t sendLength = US_MIN(remaining, sUsbDeviceDescriptor.bMaxPacketSize);
					halUsbDeviceWriteTxIsr(aDriver, 0, sDriverState.sendBuffer, sendLength,
						!(aContext->onTxIsr.transactionFlags & HalUsbTransactionData1)); // TODO: DATA0?
					sDriverState.sendBuffer += sendLength;
				}
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

