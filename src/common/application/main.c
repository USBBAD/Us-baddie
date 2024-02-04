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

void onAdcIsr()
{
	volatile uint16_t *buffer = dmaGetBufferIsr(1, 1);
}

int main(void)
{
	targetInitialize();
	uartConfigure(1, 115200);
	dmaSetIsrHook(1, 1, onAdcIsr);
	adcStart();

	while (1) {
//		uartTryPuts(1, "Hello, world!");
	}
}
