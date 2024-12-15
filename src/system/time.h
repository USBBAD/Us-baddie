/**
 * time.h
 *
 * Created on: September 14, 2024
 *     Author: Dmitry Murashov
 */

#ifndef SRC_SYSTEM_TIME_H_
#define SRC_SYSTEM_TIME_H_

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

uint64_t timeGetUptimeUs();

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline uint64_t timeBusywaitUs(uint64_t aDurationUs)
{
	uint64_t stamp = timeGetUptimeUs() + aDurationUs;
	while (timeGetUptimeUs() < stamp) {
		/* Empty */
	}
}

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif /* SRC_SYSTEM_TIME_H_ */

