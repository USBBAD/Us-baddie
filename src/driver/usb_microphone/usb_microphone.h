//
// usb_microphone.h
//
// Created on: July 26, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_DRIVER_USB_MICROPHONE_USB_MICROPHONE_H_
#define SRC_DRIVER_USB_MICROPHONE_USB_MICROPHONE_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include "driver/usb_microphone/usb_control.h"
#include "driver/usb_microphone/usb_isoch.h"
#include <stddef.h>
#include <stdint.h>

/****************************************************************************
* Pre-processor Definitions
****************************************************************************/

/****************************************************************************
* Public Types
****************************************************************************/

#ifndef __ASSEMBLY__

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
* Public Function Prototypes
****************************************************************************/

void usbMicrophoneSetMonoPcm16Buffer(const uint16_t *aBuffer, size_t aSize);
void usbMicrophoneSetEnabled(int aEnabled);
int usbMicrophoneIsEnabled();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline void usbMicrophoneRegisterUsbDriver()
{
	initializeEp0UsbHalDeviceDriver();
	initializeEp1UsbHalDeviceDriver();
}

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif  // SRC_DRIVER_USB_MICROPHONE_USB_MICROPHONE_H_
