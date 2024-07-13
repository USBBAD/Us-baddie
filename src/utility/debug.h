//
// debug.h
//
// Created on: March 23, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#ifndef SRC_COMMON_UTILITY_DEBUG_H_
#define SRC_COMMON_UTILITY_DEBUG_H_

#include "utility/usvprintf.h"
#include <stdint.h>

typedef void(*UsDebugCallable)(const void *);

/// \brief Each module is associated w/ a token
/// \returns token `>=0`, if successful
/// \pre No checks for contexts should be assumed, usDebugRegisterToken should be called once for each token
int usDebugRegisterToken(const char *aContext);

/// \brief Puts task into a queue
/// \returns >0, if successful
/// \post Will produce an error message, when failed
/// \pre `usDebugRegisterToken` must be called before to get `aToken`
int usDebugAddTask(int aToken, UsDebugCallable aCallable, const void *aArg);

/// \brief Pushes message w/o the need for formatting
/// \details CR, NL symbols will be added automatically
int usDebugPushMessage(int aToken, const char *aMessage);

void usDebugIterDebugLoop();
void usDebugPrintU8Array(const void *aData, size_t aDataLength);
void usDebugPrintU16Array(const void *aData, size_t aDataLength);
void usDebugPrintU32Array(const void *aData, size_t aDataLength);

static inline void usDebugPrintHex8(uint8_t hex)
{
	extern const char gHexmap[16];
	gVprintfCallback(&gHexmap[hex >> 4], 1);
	gVprintfCallback(&gHexmap[hex & 0x0f], 1);
}

static inline void usDebugPrintHex16(uint16_t hex)
{
	extern const char gHexmap[16];
	gVprintfCallback(&gHexmap[(hex & 0xf000) >> 12], 1);
	gVprintfCallback(&gHexmap[(hex & 0x0f00) >> 8], 1);
	gVprintfCallback(&gHexmap[(hex & 0x00f0) >> 4], 1);
	gVprintfCallback(&gHexmap[(hex & 0x000f)], 1);
}

static inline void usDebugPrintHex32(uint32_t hex)
{
	extern const char gHexmap[16];
	gVprintfCallback(&gHexmap[(hex & 0xf0000000) >> 28], 1);
	gVprintfCallback(&gHexmap[(hex & 0x0f000000) >> 24], 1);
	gVprintfCallback(&gHexmap[(hex & 0x00f00000) >> 20], 1);
	gVprintfCallback(&gHexmap[(hex & 0x000f0000) >> 16], 1);
	gVprintfCallback(&gHexmap[(hex & 0x0000f000) >> 12], 1);
	gVprintfCallback(&gHexmap[(hex & 0x00000f00) >> 8], 1);
	gVprintfCallback(&gHexmap[(hex & 0x000000f0) >> 4], 1);
	gVprintfCallback(&gHexmap[(hex & 0x0000000f)], 1);
}

#endif  // SRC_COMMON_UTILITY_DEBUG_H_

