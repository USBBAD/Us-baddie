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

void onAdcDmaIsr()
{
	volatile uint16_t *buffer = dmaGetBufferIsr(1, 1);
	volatile uint16_t ch2 = buffer[1];
	volatile uint16_t ch1 = buffer[0];
}

int main(void)
{
	targetInitialize();
	uartConfigure(1, 115200);

	while (1) {
		uartTryPuts(1, "Hello, world!");
	}
}
