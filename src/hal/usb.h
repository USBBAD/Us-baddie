//
// usb.h
//
// Created on: May 19, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT gmail DOT com)
//

#ifndef SRC_HAL_USB_H_
#define SRC_HAL_USB_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include <stddef.h>
#include <stdint.h>

/****************************************************************************
* Pre-processor Definitions
****************************************************************************/

/****************************************************************************
* Public Types
****************************************************************************/

#ifndef __ASSEMBLY__

// USB SETUP transaction constants
enum {
	// bmRequestType field values
	UsbBmRequestTypeRecipientMask = 0b1111,
	UsbBmRequestTypeRecipientDevice = 0,
	UsbBmRequestTypeRecipientInterface = 1,
	UsbBmRequestTypeRecipientEndpoint = 2,
	// bRequest field values
	UsbBRequestGetStatus = 0,
	UsbBRequestClearFeature = 1,
	UsbBRequestSetFeature = 3,
	UsbBRequestSetAddress = 5,
	UsbBRequestGetDescriptor = 6,
	UsbBRequestSetDescriptor = 7,
	UsbBRequestGetConfiguration = 8,
	UsbBRequestSetConfiguration = 9,
};

typedef enum {
	HalUsbTransactionData1 = 1 << 0,  ///< If set, DATA1. Else DATA0
	HalUsbTransactionSetup = 1 << 1,  ///< If set, SETUP transaction is taking place
	HalUsbTransactionOut = 1 << 2,  ///< If set, OUT transaction is taking place
	HalUsbTransactionIn = 1 << 3,  ///< If set, OUT transaction is taking place
} HalUsbTransaction;

/// \brief A supplement providing additional information (just in case)
union HalUsbDeviceContextVariant {
	struct {
		uint8_t endpointId;
		HalUsbTransaction transactionFlags;
	} onRxIsr;
};

struct HalUsbDeviceDriver {
	/// \var Implementation's private fields. OPTIONAL
	void *priv;

	/// \var onRx callback
	/// \warning MAY be called from ISR. Therefore, the implementation MUST
	/// NOT introduce additional delays.
	/// \param aContext Implementation must use `onRxIsr` union member
	/// \post If the communication is to be continued, the application MUST
	/// specify which type of DATAx transfer to expect (DATA0, DATA1)
	/// next time
	void (*onRxIsr)(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *aContext, const void *aBuffer,
		size_t aSize);
};

/****************************************************************************
* Public Data
****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

/****************************************************************************
* Public Function Prototypes
****************************************************************************/

/// \brief To be implemented on a particular target
void halUsbDeviceRegisterDriver(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint);

/// \brief Puts a certain number of bytes into USB TX. MAY
/// involve delayed sending. To be implemented on a particular
/// platform. WILL be called from ISR
void halUsbDeviceWriteTxIsr(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint, const void *aBuffer, size_t aSize,
	int aIsData1);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif  /* __ASSEMBLY__ */

#endif  // SRC_HAL_USB_H_
