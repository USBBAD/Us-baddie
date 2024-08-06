/**
 * stub.h
 *
 * Created on: July 31, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_DRIVER_USB_MICROPHONE_STUB_H_
#define SRC_DRIVER_USB_MICROPHONE_STUB_H_

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

/**
 * @brief usbMicrophoneInitStub Initializes stub microphone functionality
 * which transmits some artificially-generated soundwave
 */
void usbMicrophoneInitStub();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif /* SRC_DRIVER_USB_MICROPHONE_STUB_H_ */
