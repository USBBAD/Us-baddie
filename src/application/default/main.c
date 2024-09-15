/*
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

#include "application/default/target.h"
#include "driver/usb_microphone/stereo.h"
#include "driver/usb_microphone/stub.h"
#include "driver/usb_microphone/usb_microphone.h"
#include "hal/adc.h"
#include "hal/dma.h"
#include "hal/uart.h"
#include "system/time.h"
#include "utility/debug.h"
#include "utility/ushelp.h"
#include "utility/usvprintf.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

static uint16_t audioBuffer[64] = {0}; /**< 64 is the required size for the buffer (see usb_microphone driver, usb_control.c) */
static struct UsbMicrophoneStereo sUsbMicrophoneStereo = {
	.buffer = audioBuffer,
	.size = US_ARRAY_SIZE(audioBuffer),
};

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

void onAdcDmaIsr()
{
	usDebugPushMessage(0, "got DMA ISR");
	uint16_t *buffer = dmaGetBufferIsr(1, 1);
	audioBuffer[0] = buffer[0];
	audioBuffer[1] = buffer[1];
}

size_t uartPuts(const char *aBuffer, size_t aBufferLen)
{
	if (uartTryPutsLen(1, aBuffer, aBufferLen)) {
		uartBusyWaitForWritten(1);

		return aBufferLen;
	}

	return 0U;
}

static void printAudio(const void *aArg)
{
	usvprintf("Audio test L %u R %u\r\n", audioBuffer[0], audioBuffer[1]);
}

static void printStarted(const void *aArg)
{
	usvprintf("Application started\r\n");
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

static void taskRunAudio()
{
	const uint64_t kMultisamplePeriodUs = 200000;
	uint64_t now = timeGetUptimeUs();
	uint64_t nextSampleTimestamp = now + kMultisamplePeriodUs;
	while (1) {
		now = timeGetUptimeUs();
		if (now > nextSampleTimestamp) {
			nextSampleTimestamp = now + kMultisamplePeriodUs;
			adcStart();
		} else {
			usvprintf("Audio L %u R %u\r\n", audioBuffer[0], audioBuffer[1]);
		}
	}
}

int main(void)
{
	int val = 10;
	/* Hardware initialization */
	targetInitialize(audioBuffer, US_ARRAY_SIZE(audioBuffer), onAdcDmaIsr);
	uartConfigure(1, 921600);

	/* Configure logging */
	usvprintfSetPuts(uartPuts);
	dmaSetIsrHook(1, 1, onAdcDmaIsr);
	adcStart();
	usDebugAddTask(0, printStarted, 0);
	usDebugPushMessage(0, "System is up");
	usbMicrophoneInitUsbDriver();
	usbMicrophoneInitStub();
	usbMicrophoneInitStereo(&sUsbMicrophoneStereo);
	usDebugSetLed(0, 0);

	taskRunAudio();
	taskRunSystick();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
