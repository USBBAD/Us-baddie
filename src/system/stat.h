/**
 * stat.h
 *
 * Created on: December 08, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_SYSTEM_STAT_H_
#define SRC_SYSTEM_STAT_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include <stdint.h>

/****************************************************************************
* Pre-processor Definitions
****************************************************************************/

/****************************************************************************
* Public Types
****************************************************************************/

enum StatUsbErr {
	StatUsbErrNoEpBuffer = 0x1,
};

struct Stat {
	uint32_t usbIsochPackets; /**< # Of USB ISOCH packets */
	enum StatUsbErr usbErr;
	uint32_t usbIsochB; /**< Amount of transferred data in USB isoch packets, [B] */
};

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

extern struct Stat gSysStat;

/****************************************************************************
* Public Function Prototypes
****************************************************************************/

void sysStatPrint();
void sysStatReset();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif /* SRC_SYSTEM_STAT_H_ */

