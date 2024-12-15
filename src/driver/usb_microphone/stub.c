/**
 * stub.c
 *
 * Created on: July 31, 2024
 *     Author: Dmitry Murashov
 */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define US_RANGE_A (-39)
#define US_RANGE_B (39)
#define US_SCALE (1)
#define US_OFFSET (0)

#define USBAD_DEBUG_REGDUMP_FIFO_SIZE (10)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/usb_microphone.h"
#include "utility/debug.h"
#include "utility/debug_regdump.h"
#include "utility/ushelp.h"
#include <math.h>
#include <stdint.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

/** \struct WaveGenerationState parameterizes the process of wave generation.
 * It helps iterating along X axis. The wave function is a function of some
 * sort.
 */
struct WaveGenerationState {
	int32_t counter;
	int32_t rangeA;
	int32_t rangeB;
	int32_t scale;
	int32_t offset;
	int32_t increment;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static uint16_t generateSample();
void generate();
void onEnabledStateChangedIsr(int aEnabled);
void onChunkTransmitted();

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct WaveGenerationState sWaveGenerationState = {
	.counter = US_RANGE_A,
	.rangeA = US_RANGE_A,
	.rangeB = US_RANGE_B,
	.scale = US_SCALE,
	.offset = US_OFFSET,
	.increment = 1,
};

/** \struct UsbMicrophoneHook is a high-level driver that handles buffer management
 * during audio transmission
 */
static struct UsbMicrophoneHook sUsbMicrophoneHook = {
	.onChunkTransmitted = onChunkTransmitted,
	.onEnabledStateChangedIsr = onEnabledStateChangedIsr,
};

uint16_t sMonoPcmBuffer[32]; /**< Stores generated samples */

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static uint16_t generateSample()
{
	/* x * (xx / 512 + 3), imitates sine wave without using floats */
	const int32_t value = sWaveGenerationState.counter * ((sWaveGenerationState.counter
		* sWaveGenerationState.counter + 1536) >> 9) * US_SCALE + US_OFFSET;
	sWaveGenerationState.counter += sWaveGenerationState.increment;

	if (sWaveGenerationState.counter > US_RANGE_B) {
		sWaveGenerationState.counter = US_RANGE_A;
	}

	return US_CLAMP(0, 0xFFFF, value);
}

void generate()
{
#if 0
	usDebugPushMessage(0, "Generating");
	for (int i = 0; i < US_ARRAY_SIZE(sMonoPcmBuffer); ++i) {
		sMonoPcmBuffer[i] = generateSample();
	}
#else
	for (int i = 0; i < US_ARRAY_SIZE(sMonoPcmBuffer); ++i) {
		sMonoPcmBuffer[i] = 0xcafe;
	}
#endif
}

void onEnabledStateChangedIsr(int aEnabled)
{
	if (aEnabled) {
		usbMicrophoneSetMonoPcm16Buffer(&sMonoPcmBuffer[0], US_ARRAY_SIZE(sMonoPcmBuffer));
	}
}

void onChunkTransmitted()
{
	if (usbMicrophoneIsEnabled()) {
		usbMicrophoneSetMonoPcm16Buffer(&sMonoPcmBuffer[0], US_ARRAY_SIZE(sMonoPcmBuffer));
	}
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbMicrophoneInitStub()
{
	debugRegdumpInitialize("");
	usbMicrophoneSetHook(&sUsbMicrophoneHook);
	generate();
}
