//
// usvprintf.h
//
// Created on: March 17, 2024
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef SRC_COMMON_UTILITY_VPRINTF_H_
#define SRC_COMMON_UTILITY_VPRINTF_H_

#include "cbprintf.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

extern puts_t *gVprintfCallback;

/// \brief Sets a callback
static inline void usvprintfSetPuts(puts_t aCallback)
{
	gVprintfCallback = aCallback;
}

int usvprintf(const char *fmt, ...) __attribute__((format(usvprintf, 1, 2)));

#if __cplusplus
}
#endif // __cplusplus

#endif  // SRC_COMMON_UTILITY_VPRINTF_H_
