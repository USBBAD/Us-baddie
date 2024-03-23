//
// debug.c
//
// Created on: March 23, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#define US_DEBUG_MAX_TOKENS (10)
#define US_DEBUG_MAX_TOKEN_SLOTS (3)
#define US_DEBUG_FAIL_TOKEN_SLOT_ID (3)

#include "utility/debug.h"
#include "utility/usvprintf.h"
#include <string.h>

static size_t sCounter = 0U;

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
	struct TokenSlot tokenSlots[US_DEBUG_MAX_TOKEN_SLOTS + 1];
	const char *context;
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
	for (int i = 0; i < US_DEBUG_MAX_TOKENS; ++i) {
		if (sDebugContext[i].context == 0) {
			sDebugContext[i].context = aContext;

			return i;
		}
	}

	return -1;
}

int usDebugAddTask(int aToken, UsDebugCallable aCallable, const void *aArg)
{
	if (aToken < 0) {
		return -1;
	}

	for (int i = 0; i < US_DEBUG_MAX_TOKEN_SLOTS + 1; ++i) {
		if (i == US_DEBUG_FAIL_TOKEN_SLOT_ID) {
			sDebugContext[aToken].tokenSlots[i].callable = printFailedToAddTask;
			sDebugContext[aToken].tokenSlots[i].arg = sDebugContext[aToken].context;
			sDebugContext[aToken].tokenSlots[i].counter = sCounter++;

			return -1;
		} else  if (sDebugContext[aToken].tokenSlots[i].callable == 0) {
			sDebugContext[aToken].tokenSlots[i].callable = aCallable;
			sDebugContext[aToken].tokenSlots[i].arg = aArg;
			sDebugContext[aToken].tokenSlots[i].counter = sCounter++;

			return 1;
		}
	}

	return -1;
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

		for (int j = 0; j < US_DEBUG_MAX_TOKEN_SLOTS + 1; ++j) {
			if (sDebugContext[i].tokenSlots[j].callable) {
				usvprintf("%u [%s] ", sDebugContext[i].tokenSlots[j].counter, sDebugContext[i].context);
				sDebugContext[i].tokenSlots[j].callable(sDebugContext[i].tokenSlots[j].arg);
				sDebugContext[i].tokenSlots[j].callable = 0;
				sDebugContext[i].tokenSlots[j].arg = 0;
			}
		}
	}
}
