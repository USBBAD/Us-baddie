//
// usb_isoch.h
//
// Created on: July 26, 2024
//     Author: Dmitry Murashov
//
// Handles isochronous data transfer

#ifndef SRC_DRIVER_USB_MICROPHONE_USB_ISOCH_H_
#define SRC_DRIVER_USB_MICROPHONE_USB_ISOCH_H_

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
* Public Function Prototypes
****************************************************************************/

void initializeEp1UsbHalDeviceDriver();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */


#endif  // SRC_DRIVER_USB_MICROPHONE_USB_ISOCH_H_
