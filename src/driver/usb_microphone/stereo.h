/**
 * stereo.h
 *
 * Created on: September 01, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_DRIVER_USB_MICROPHONE_STEREO_H_
#define SRC_DRIVER_USB_MICROPHONE_STEREO_H_

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

struct UsbMicrophoneStereo {
	const uint16_t *buffer;
	size_t size;
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
* Public Function Prototypes
****************************************************************************/

void usbMicrophoneInitStereo(struct UsbMicrophoneStereo *aStereo);

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif /* SRC_DRIVER_USB_MICROPHONE_STEREO_H_ */
