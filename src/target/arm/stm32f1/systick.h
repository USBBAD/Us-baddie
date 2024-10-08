//
// systick.h
//
// Created on: September 14, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_ARM_STM32F1_SYSTICK_H_
#define SRC_TARGET_ARM_STM32F1_SYSTICK_H_

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

void systickInit(uint32_t aResolutionUs);

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif // SRC_TARGET_ARM_STM32F1_SYSTICK_H_
