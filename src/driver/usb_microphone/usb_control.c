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
	.bLength = 0x12,
	.bDescriptorType = 1, // Device descriptor
	.bcdUsb = 0x0200, // TODO: I use an example from USB Audio specs, and it uses USB 1.0 The 1.0 is also limited to 8 byte packages. However, the spec recommends adhering to USB 2.0
	.bDeviceClass = 0x00, // TODO
	.bDeviceSubClass = 0x00, // TODO
	.bDeviceProtocol = 0x00, // TODO
	.bMaxPacketSize = 64,
	.idVendor = 0x0483, // TODO
	.idProduct = 0x5722, // TODO
	.bcdDevice = 1, // 0.01, beta version
	.iManufacturer = 1, // String #1 contains the manufacturer's name
	.iProduct = 2, // String #2 contains the prosduct's title
	.iSerialNumber = 0,
	.bNumConfigurations = 1, // One configuration
};

static const uint8_t sUsbMicrophoneConfigurationDescriptor[] = {
	// Configuration header (standard)

	0x09, // bLength Size of this descriptor, in bytes.
	0x02, // bDescriptorType CONFIGURATION descriptor.
	0x00, 0x64, // wTotalLength Length of the total configuration block, including this descriptor, in bytes.
	0x02, // bNumInterfaces Two interfaces.
	0x01, // bConfigurationValue ID of this configuration.
	0x00, // iConfiguration Unused.
	0x80, // bmAttributes Bus Powered device, not Self Powered, no Remote wakeup capability.
	0x0A, // MaxPower 20 mA Max. power consumption.

	// USB Microphone Standard AC Interface Descriptor

	0x09, // bLength Size of this descriptor, in bytes.
	0x04, // bDescriptorType INTERFACE descriptor.
	0x00, // bInterfaceNumber Index of this interface.
	0x00, // bAlternateSetting Index of this setting.
	0x00, // bNumEndpoints 0 endpoints.
	0x01, // bInterfaceClass AUDIO.
	0x01, // bInterfaceSubclass AUDIO_CONTROL.
	0x00, // bInterfaceProtocol Unused.
	0x00, // iInterface Unused.

	// USB Microphone Class-specific AC Interface Descriptor

	0x09, // bLength Size of this descriptor, in bytes.
	0x24, // bDescriptorType CS_INTERFACE.
	0x01, // bDescriptorSubtype HEADER subtype.
	0x01, 0x00, // bcdADC Revision of class specification - 1.0
	0x00, 0x1E, // wTotalLength Total size of class specific descriptors.
	0x01, // bInCollection Number of streaming interfaces.
	0x01, // baInterfaceNr(1) AudioStreaming interface 1 belongs to this AudioControl interface.

	// USB Microphone Input Terminal Descriptor

	0x0C, // bLength Size of this descriptor, in bytes.
	0x24, // bDescriptorType CS_INTERFACE.
	0x02, // bDescriptorSubtype INPUT_TERMINAL subtype.
	0x01, // bTerminalID ID of this Input Terminal.
	0x02, 0x01, // wTerminalType Terminal is Microphone.
	0x00, // bAssocTerminal No association.
	0x01, // bNrChannels One channel.
	0x00, 0x00, // wChannelConfig Mono sets no position bits.
	0x00, // iChannelNames Unused.
	0x00, // iTerminal Unused.

	// USB Microphone Output Terminal Descriptor

	0x09, // bLength Size of this descriptor, in bytes.
	0x24, // bDescriptorType CS_INTERFACE.
	0x03, // bDescriptorSubtype OUTPUT_TERMINAL subtype.
	0x02, // bTerminalID ID of this Output Terminal.
	0x01, 0x01, // wTerminalType USB Streaming.
	0x00, // bAssocTerminal Unused.
	0x01, // bSourceID From Input Terminal.
	0x00, // iTerminal Unused.

	//  AudioStreaming Interface Descriptor

	0x09, // bLength Size of this descriptor, in bytes.
	0x04, // bDescriptorType INTERFACE descriptor.
	0x01, // bInterfaceNumber Index of this interface.
	0x00, // bAlternateSetting Index of this alternate setting.
	0x00, // bNumEndpoints 0 endpoints.
	0x01, // bInterfaceClass AUDIO.
	0x02, // bInterfaceSubclass AUDIO_STREAMING.
	0x00, // bInterfaceProtocol Unused.
	0x00, // iInterface Unused.

	//  Standard AS Interface Descriptor

	0x09, // bLength Size of this descriptor, in bytes.
	0x04, // bDescriptorType INTERFACE descriptor.
	0x01, // bInterfaceNumber Index of this interface.
	0x01, // bAlternateSetting Index of this alternate setting.
	0x01, // bNumEndpoints One endpoint.
	0x01, // bInterfaceClass AUDIO.
	0x02, // bInterfaceSubclass AUDIO_STREAMING.
	0x00, // bInterfaceProtocol Unused.
	0x00, // iInterface Unused.

	// Class-specific AS General Interface Descriptor

	0x07, // bLength Size of this descriptor, in bytes.
	0x24, // bDescriptorType CS_INTERFACE descriptor.
	0x01, // bDescriptorSubtype GENERAL subtype.
	0x02, // bTerminalLink Unit ID of the Output Terminal.
	0x01, // bDelay Interface delay.
	0x00, 0x01, // wFormatTag PCM Format.

	//  USB Microphone Type I Format Type Descriptor

	0x0B, // bLength Size of this descriptor, in bytes.
	0x24, // bDescriptorType CS_INTERFACE descriptor.
	0x02, // bDescriptorSubtype FORMAT_TYPE subtype.
	0x01, // bFormatType FORMAT_TYPE_I.
	0x01, // bNrChannels One channel.
	0x02, // bSubFrameSize Two bytes per audio subframe.
	0x10, // bBitResolution 16 bits per sample.
	0x01, // bSamFreqType One frequency supported.
	0x00, 0x1F, 0x40, // tSamFreq 8000Hz.

	// Standard Endpoint Descriptor

	0x09, // bLength Size of this descriptor, in bytes.
	0x24, // bDescriptorType CS_INTERFACE.
	0x03, // bDescriptorSubtype OUTPUT_TERMINAL subtype.
	0x02, // bTerminalID ID of this Output Terminal.
	0x01, 0x01, // wTerminalType USB Streaming.
	0x00, // bAssocTerminal Unused.
	0x01, // bSourceID From Input Terminal.
	0x00, // iTerminal Unused.

	0x09, // bLength Size of this descriptor, in bytes.
	0x05, // bDescriptorType ENDPOINT descriptor.
	0x81, // bEndpointAddress IN Endpoint 1.
	0x01, // bmAttributes Isochronous, not shared.
	0x00, 0x10, // wMaxPacketSize 16 bytes per packet.
	0x01, // bInterval One packet per frame.
	0x00, // bRefresh Unused.
	0x00, // bSynchAddress Unused.

	// USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor

	0x07, // bLength Size of this descriptor, in bytes.
	0x25, // bDescriptorType CS_ENDPOINT descriptor
	0x01, // bDescriptorSubtype GENERAL subtype.
	0x00, // bmAttributes No sampling frequency control, no pitch control, no packet padding.
	0x00, // bLockDelayUnits Unused.
	0x00, 0x00, // wLockDelay Unused.
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
					halUsbDeviceWriteTxIsr(aDriver, 0, (const void *)&sUsbMicrophoneConfigurationDescriptor[0],
						aSetupTransaction->wLength, 1);
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

