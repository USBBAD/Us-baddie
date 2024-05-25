//
// debug_regdump.h
//
// Created on: May 25, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_UTILITY_DEBUG_REGDUMP_H_
#define SRC_UTILITY_DEBUG_REGDUMP_H_

#include "utility/debug.h"
#include "utility/fifo.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/// \def Parameterizes header
#ifndef USBAD_DEBUG_REGDUMP_FIFO_SIZE
#  error Numeric USBAD_DEBUG_REGDUMP_FIFO_SIZE must be defined to specify Fifo queue length
#endif  // USBAD_DEBUG_REGDUMP_FIFO_SIZE

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct I32Context {
	const char *title;
	uint32_t value;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void debugRegdumpPrintI32Context(const void *aContext);
static void debugRegdumpEnqueueI32Context(const char *aTitle, uint32_t aValue);

/// \returns registered token
static void debugRegdumpInitialize(const char *aToken);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct I32Context sI32Context[USBAD_DEBUG_REGDUMP_FIFO_SIZE] = {{0}};
static Fifo sI32ContextFifo;
static int sDebugRegdumpDebugToken = -1;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline int getDebugToken()
{
	return sDebugRegdumpDebugToken;
}

static void debugRegdumpPrintI32Context(const void *a)
{
	struct I32Context *i32Context;
	(void)a;

	while ((i32Context = fifoPop(&sI32ContextFifo)) != 0) {
		usvprintf("%s 0x%08X\r\n", i32Context->title, i32Context->value);
	}
}

static void debugRegdumpEnqueueI32Context(const char *aTitle, uint32_t aValue)
{
	struct I32Context *context = fifoPush(&sI32ContextFifo);

	if (context) {
		*context = (struct I32Context) {
			.title = aTitle,
			.value = aValue,
		};
		usDebugAddTask(getDebugToken(), debugRegdumpPrintI32Context, 0);
	}
}

static void debugRegdumpInitialize(const char *aToken)
{
	sDebugRegdumpDebugToken = usDebugRegisterToken(aToken);
	fifoInitialize(&sI32ContextFifo, &sI32Context, USBAD_DEBUG_REGDUMP_FIFO_SIZE, sizeof(struct I32Context));
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#else
#  error This file only contains static functions, and MUST ONLY be included from C-sources once. An attempt to include it twice has been made
#endif  // SRC_UTILITY_DEBUG_REGDUMP_H_
