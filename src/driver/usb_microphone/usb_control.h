//
// usb_control.h
//
// Created on: May 19, 2024
//     Author: Dmitry Murashov
//
// Implements USB device control endpoint driver
//

#ifndef SRC_TARGET_USBAD_SRC_USB_CONTROL_H_
#define SRC_TARGET_USBAD_SRC_USB_CONTROL_H_

/****************************************************************************
* Included Files
****************************************************************************/

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
 * Inline Functions
 ****************************************************************************/

/****************************************************************************
* Public Function Prototypes
****************************************************************************/

void initializeEp0UsbHalDeviceDriver();

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif  // SRC_TARGET_USBAD_SRC_USB_CONTROL_H_

