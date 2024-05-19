//
// UsbDevice.hpp
//
// Created on: May 12, 2024
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef SRC_HAL_USB_USBDEVICE_HPP_
#define SRC_HAL_USB_USBDEVICE_HPP_

#include <array>

class UsbDevice {
public:
	virtual void onRx(uint8_t aEndpoint, const void *aRx, uint8_t aLength) = 0;
	virtual ~UsbDevice() = default;
};

#endif  // SRC_HAL_USB_USBDEVICE_HPP_
