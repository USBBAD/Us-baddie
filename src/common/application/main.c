//
// main.c
//
// Created: 2023-10-11
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

#include "hal/uart.h"
#include "target/target.h"

int main(void)
{
	targetInitialize();
	uartConfigure(1, 115200);

	while (1) {
	}
}
