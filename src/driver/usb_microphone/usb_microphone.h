//
// usb_microphone.h
//
// Created on: July 26, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_DRIVER_USB_MICROPHONE_USB_MICROPHONE_H_
#define SRC_DRIVER_USB_MICROPHONE_USB_MICROPHONE_H_

#include "driver/usb_microphone/usb_control.h"
#include "driver/usb_microphone/usb_isoch.h"

static inline void usbMicrophoneRegisterUsbDriver()
{
	initializeEp0UsbHalDeviceDriver();
	initializeEp1UsbHalDeviceDriver();
}

#endif  // SRC_DRIVER_USB_MICROPHONE_USB_MICROPHONE_H_

