//
// usb.h
//
// Created on: May 19, 2024
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef SRC_HAL_USB_H_
#define SRC_HAL_USB_H_

#include <stddef.h>

/// \brief A supplement providing additional information (just in case)
union HalUsbDeviceContextVariant {
};

struct HalUsbDeviceDriver {
	/// \var Implementation's private fields. OPTIONAL
	void *priv;

	/// \var onRx callback
	/// \warning MAY be called from ISR. Therefore, the implementation MUST
	/// NOT introduce additional delays.
	void (*onRx)(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *, const void *aBuffer, size_t aSize);
};

/// \brief To be implemented on a particular target
void halUsbDeviceRegisterDriver(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint);

#endif  // SRC_HAL_USB_H_

