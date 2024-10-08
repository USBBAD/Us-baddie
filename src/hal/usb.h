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
	UsbBRequestGetStatus = 0x00,
	UsbBRequestClearFeature = 0x01,
	UsbBRequestSetFeature = 0x03,
	UsbBRequestSetAddress = 0x05,
	UsbBRequestGetDescriptor = 0x06,
	UsbBRequestSetDescriptor = 0x07,
	UsbBRequestGetConfiguration = 0x08,
	UsbBRequestSetConfiguration = 0x09,
	UsbBRequestGetInterface = 0x0a,
	UsbBRequestSetInterface = 0x0b,
};

typedef enum {
	HalUsbTransactionData1 = 1 << 0,  /** < If set, DATA1. Else DATA0 */
	HalUsbTransactionSetup = 1 << 1,  /** < If set, SETUP transaction is taking place */
	HalUsbTransactionOut = 1 << 2,  /** < If set, OUT transaction is taking place */
	HalUsbTransactionIn = 1 << 3,  /** < If set, OUT transaction is taking place */
} HalUsbTransaction;

enum HalUsbEpState {
	HalUsbEpStateDisabled = 0, /**< The EP will not respond to any command */
	HalUsbEpStateNak, /**< EP requests will be NAKed */
	HalUsbEpStateStall, /**< EP requests will be STALLed */
	HalUsbEpStateValid, /**< The EP is ready to process IN or OUT packet. When IN transactions are involved, the driver MUST set the EP to valid automatically */
};

/// \brief A supplement providing additional information (just in case)
union HalUsbDeviceContextVariant {
	struct {
		uint8_t endpointId;
		HalUsbTransaction transactionFlags;
	} onRxIsr;
	struct {
		uint8_t endpointId;
		HalUsbTransaction transactionFlags;
	} onTxIsr;
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

	/// \var On-successful-TX callback
	/// \details Gets invoked whenever a TX transaction has been finished
	void (*onTxIsr)(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *aContext);
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
/// \post The EP state is automatically set to HalUsbEpStateValid
void halUsbDeviceWriteTxIsr(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint, const void *aBuffer, size_t aSize,
	int aIsData1);
void halUsbDeviceSetAddress(struct HalUsbDeviceDriver *aDriver, uint8_t aAddress);

/**
 * @brief halUsbSetEpState sets EP state
 * @param aDriver
 * @param aEpState
 */
void halUsbSetEpState(struct HalUsbDeviceDriver *aDriver, uint8_t aEp, enum HalUsbEpState aEpStateIn,
	enum HalUsbEpState aEpStateOut);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif  /* __ASSEMBLY__ */

#endif  // SRC_HAL_USB_H_
