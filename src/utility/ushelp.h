//
// ushelp.h
//
// Created on: Dec 10, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

/// \brief Various helpers

#ifndef USBAD_USHELP_H_
#define USBAD_USHELP_H_

#define US_MIN(a, b) ((a) > (b) ? (b) : (a))
#define US_MAX(a, b) ((a) < (b) ? (b) : (a))
#define US_GET_BIT(value, mask, offset) ((value & mask) >> offset)
#define US_CLAMP(a, b, val) (US_MAX(a, US_MIN(b, val)))
#define US_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define US_ASSERT(a)

#include <stddef.h>
#include <stdint.h>

#define PRIV_OP_SUM(OUT, VAL, ARG1) (OUT += VAL)
#define PRIV_OP_MAX(OUT, VAL, ARG1) (OUT = OUT > VAL ? OUT : VAL)
#define PRIV_OP_MIN(OUT, VAL, ARG1) (OUT = OUT < VAL ? OUT : VAL)
#define PRIV_US_ARR_SUM_IMPL(FNAME, TYPEOUT, TYPEIN, OP) \
static inline TYPEOUT FNAME(const TYPEIN *aBuf, size_t aSz) \
{ \
	US_ASSERT(aBuf != 0);\
	TYPEOUT res = 0; \
	for (size_t i = 0; i < aSz; ++i) { \
		OP(res, aBuf[i], 0); \
	} \
	return res; \
}
PRIV_US_ARR_SUM_IMPL(usSumU16, uint32_t, uint16_t, PRIV_OP_SUM)
PRIV_US_ARR_SUM_IMPL(usMaxU16, uint16_t, uint16_t, PRIV_OP_MAX)
PRIV_US_ARR_SUM_IMPL(usMinU16, uint16_t, uint16_t, PRIV_OP_MIN)

#endif // USBAD_USHELP_H_
