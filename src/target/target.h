//
// target.h
//
// Created on: 2023-10-11
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL>)
//

#ifndef USBAD_SRC_COMMON_TARGET_TARGET_H_
#define USBAD_SRC_COMMON_TARGET_TARGET_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include "hal/adc.h"

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
/// \brief Performs all the target-dependent initialization
/// \returns Target handle, if successful. 0 otherwise
void targetInitialize();
void memoryInitialize();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif  // USBAD_SRC_COMMON_TARGET_TARGET_H_
