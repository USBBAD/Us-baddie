//
// main.c
//
// Created: 2023-10-11
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

#include "driver/usb_microphone/stub.h"
#include "driver/usb_microphone/usb_microphone.h"
#include "hal/adc.h"
#include "hal/dma.h"
#include "hal/uart.h"
#include "system/time.h"
#include "target/target.h"
#include "utility/debug.h"
#include "utility/usvprintf.h"

static uint16_t audioBuffer[2] = {0};
static int sToken = -1;

void onAdcDmaIsr()
{
	usDebugPushMessage(sToken, "got DMA ISR");
	uint16_t *buffer = dmaGetBufferIsr(1, 1);
	audioBuffer[0] = buffer[0];
	audioBuffer[1] = buffer[1];
	adcStopIsr();
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
	memoryInitialize();
	sToken = usDebugRegisterToken("application");
	targetInitialize();
	uartConfigure(1, 921600);

	// Configure logging
	usvprintfSetPuts(uartPuts);
	dmaSetIsrHook(1, 1, onAdcDmaIsr);
	adcStart();
	usDebugAddTask(sToken, printStarted, 0);
	usDebugPushMessage(sToken, "System is up");
	usbMicrophoneInitUsbDriver();
	usbMicrophoneInitStub();
	usDebugSetLed(0, 0);

	taskRunAudio();
	taskRunSystick();
}
