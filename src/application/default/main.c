/**
 * main.c
 *
 * Created: 2023-10-11
 *  Author: Dmitry Murashov
 */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/audio.h"
#include "driver/usb_microphone/stub.h"
#include "driver/usb_microphone/usb_microphone.h"
#include "hal/dma.h"
#include "hal/uart.h"
#include "system/time.h"
#include "target/target.h"
#include "utility/debug.h"
#include "utility/usvprintf.h"

extern void taskRunAudio(void);

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * @todo move into target/
 */
size_t uartPuts(const char *aBuffer, size_t aBufferLen)
{
	if (uartTryPutsLen(1, aBuffer, aBufferLen)) {
		uartBusyWaitForWritten(1);
		return aBufferLen;
	}
	return 0U;
}

static void taskRunSystick()
{
	for (uint64_t prevUptime = 0;;) {
		uint64_t now = timeGetUptimeUs();
		if (now - prevUptime > 1000000UL) {
			prevUptime = now;
			usvprintf("uptime %d \r\n", (uint32_t)now);
		}
	}
}

int main(void)
{
	int val = 10;
	memoryInitialize();
	sysStatReset();
	targetInitialize();
	uartConfigure(1, 460800);

	// Configure logging
	usvprintfSetPuts(uartPuts);
	adcStart();
	usDebugPushMessage(0, "System is up");
	usbMicrophoneInitUsbDriver();
	{
		uint16_t dmaBufSz;
		void *dmaBuffer = dmaGetBufferIsr(1, 1, &dmaBufSz);
		if (dmaBuffer) {
			usbMicrophoneInitAudio(dmaBuffer, dmaBufSz);
		} else {
			while (1);
			// TODO: Panic
		}
	}
	usDebugSetLed(0, 0);

	taskRunAudio();
	taskRunSystick();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
