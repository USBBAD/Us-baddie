//
// debug.c
//
// Created on: March 23, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#define US_DEBUG_MAX_TOKENS (1)
#define US_DEBUG_MAX_REGULAR_TOKEN_SLOTS (20)
#define US_DEBUG_FAIL_TOKEN_SLOT_ID (US_DEBUG_MAX_REGULAR_TOKEN_SLOTS)
#define US_DEBUG_MAX_OVERALL_TOKEN_SLOTS (US_DEBUG_MAX_REGULAR_TOKEN_SLOTS + 1)

#include "utility/debug.h"
#include "utility/fifo.h"
#include "utility/usvprintf.h"
#include <string.h>

static size_t sCounter = 0U;
const char gHexmap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static void usDebugPrintHex8(uint8_t hex);
static void usDebugPrintHex16(uint16_t hex);
static void usDebugPrintHex32(uint32_t hex);

static void printFailedToAddTask(const void *aContext)
{
	usvprintf("Failed to add debug callback for \"%s\"\r\n", (const char *)aContext);
}

static void printNonFormattedMessage(const void *aMessage)
{
	usvprintf("%s\r\n", (const char *)aMessage);
}

struct TokenSlot {
	UsDebugCallable callable;
	const void *arg;
	size_t counter;
};

struct DebugContext {
	struct TokenSlot tokenSlots[US_DEBUG_MAX_OVERALL_TOKEN_SLOTS];
	const char *context;
	Fifo fifo;
};

static struct DebugContext sDebugContext[US_DEBUG_MAX_TOKENS] = {{
	.tokenSlots = {(struct TokenSlot){
		.callable = 0,
		.arg = 0,
	}},
	.context = 0,
}};

int usDebugRegisterToken(const char *aContext)
{
	const int i = 0;
	if (sDebugContext[i].context != 0) {
		return i;
	}
	sDebugContext[i].context = aContext;
	// Initialize FIFO for regular token slots (i.e. w/o error messages)
	fifoInitialize(&sDebugContext[i].fifo, &sDebugContext[i].tokenSlots, US_DEBUG_MAX_REGULAR_TOKEN_SLOTS,
		sizeof(struct TokenSlot));

	return i;
}

int usDebugAddTask(int aToken, UsDebugCallable aCallable, const void *aArg)
{
	struct TokenSlot *slot = 0;
	int ret = 1;
	aToken = 0;

	if (aToken < 0) {
		return -1;
	}

	slot = fifoPush(&sDebugContext[aToken].fifo);
	++sCounter;

	if (!slot) {
		aCallable = printFailedToAddTask;
		aArg = sDebugContext[aToken].context;
		ret = -1;
		slot = &sDebugContext[aToken].tokenSlots[US_DEBUG_FAIL_TOKEN_SLOT_ID];
	}

	slot->callable = aCallable;
	slot->arg = aArg;
	slot->counter = sCounter;

	return ret;
}

int usDebugPushMessage(int aToken, const char *aMessage)
{
	usDebugAddTask(aToken, printNonFormattedMessage, aMessage);
}

void usDebugIterDebugLoop()
{
	for (int i = 0; i < US_DEBUG_MAX_TOKENS; ++i) {
		if (sDebugContext[i].context == 0) {
			return;
		}

		for (int j = 0; j < US_DEBUG_MAX_OVERALL_TOKEN_SLOTS; ++j) {
			if (sDebugContext[i].tokenSlots[j].callable) {
#if 0
				usDebugPrintHex16(sDebugContext[i].tokenSlots[j].counter);
				gVprintfCallback(" ", 1);
				gVprintfCallback(sDebugContext[i].context, strlen(sDebugContext[i].context));
				gVprintfCallback(" | ", 3);
#endif
				sDebugContext[i].tokenSlots[j].callable(sDebugContext[i].tokenSlots[j].arg);
				sDebugContext[i].tokenSlots[j].callable = 0;
				sDebugContext[i].tokenSlots[j].arg = 0;
			}
		}

		fifoClear(&sDebugContext[i].fifo);
	}
}

static inline void printAs(const void *aData, size_t aDataTypeSizeof)
{
	int64_t data = 0;
}

/// \def Performs formatted print
/// \arg `rowlen` if non 0, output data will be aligned in a neat table
/// \arg `delimiter` what will be printed b/w numbers
#define USBAD_PRINT_ARRAY(numtype, data, size, format, delimiter, rowlen, callback) \
	do { \
		const numtype *ptr = (numtype *)data; \
		size_t nlCounter = 0; \
		for (; ptr != (numtype *)data + size; ++ptr) { \
			if (nlCounter) { \
				usvprintf(delimiter); \
			} else if (rowlen) { \
				gVprintfCallback("\r\n", 2); \
			} \
			nlCounter = rowlen ? (nlCounter + 1) % rowlen : nlCounter + 1; \
			gVprintfCallback("0x", 2); \
			callback(*ptr); \
		} \
	} while (0);

void usDebugPrintU8Array(const void *aData, size_t aDataLength)
{
	USBAD_PRINT_ARRAY(uint8_t, aData, aDataLength, "0x%02X", " ", 8, usDebugPrintHex8);
}

void usDebugPrintU16Array(const void *aData, size_t aDataLength)
{
	USBAD_PRINT_ARRAY(uint16_t, aData, aDataLength, "0x%04X", " ", 8, usDebugPrintHex16);
}

void usDebugPrintU32Array(const void *aData, size_t aDataLength)
{
	USBAD_PRINT_ARRAY(uint32_t, aData, aDataLength, "0x%08X", " ", 8, usDebugPrintHex32);
}
