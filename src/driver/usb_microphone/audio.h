/**
 * audio.h
 *
 * Created on: September 15, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_DRIVER_USB_MICROPHONE_AUDIO_H_
#define SRC_DRIVER_USB_MICROPHONE_AUDIO_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include <stdint.h>
#include <stddef.h>

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

void usbMicrophoneInitAudio(const uint16_t *aMonoPcmBuf, size_t aBufSize);

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif /* SRC_DRIVER_USB_MICROPHONE_AUDIO_H_ */

