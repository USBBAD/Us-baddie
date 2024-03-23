//
// main.c
//
// Created: 2023-10-11
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

#include "hal/adc.h"
#include "hal/dma.h"
#include "hal/uart.h"
#include "target/target.h"
#include "utility/usvprintf.h"

static uint16_t audioBuffer[2] = {0};

void onAdcDmaIsr()
{
	uint16_t *buffer = dmaGetBufferIsr(1, 1);
	audioBuffer[0] = buffer[0];
	audioBuffer[1] = buffer[1];
//	adcStopIsr();
}

size_t uartPuts(const char *aBuffer, size_t aBufferLen)
{
	if (uartTryPutsLen(1, aBuffer, aBufferLen)) {
		return aBufferLen;
	}

	return 0U;
}

int main(void)
{
	int val = 10;
	targetInitialize();
	uartConfigure(1, 115200);
	usvprintfSetPuts(uartPuts);
	dmaSetIsrHook(1, 1, onAdcDmaIsr);
	adcStart();

	for (size_t i = 0;; ++i) {
		usvprintf("Audio L %u R %u\r\n", audioBuffer[0], audioBuffer[1]);
	}
}
