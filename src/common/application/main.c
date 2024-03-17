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

void onAdcDmaIsr()
{
	volatile uint16_t *buffer = dmaGetBufferIsr(1, 1);
	volatile uint16_t ch2 = buffer[1];
	volatile uint16_t ch1 = buffer[0];
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

	for (size_t i = 0;; ++i) {
		usvprintf("What's up, %s? %u\n\r", "world", i);
	}
}
