//
// usb.c
//
// Created on: 15 February, 2024
//     Author: Dmitry Murashov
//
// This file has been to a significant extent derived from CMSIS USB Device
// driver template. Here is its header:
//
// Copyright (c) 2013-2020 Arm Limited. All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the License); you may
// not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//

#ifndef SRC_TARGET_STM32F103C6_SRC_USB_C_
#define SRC_TARGET_STM32F103C6_SRC_USB_C_

#include "arm/stm32f1/stm32f1_usb.h"
#include "utility/debug.h"
#include "utility/fifo.h"
#include "utility/ushelp.h"
#include "utility/usvprintf.h"
#include <stm32f103x6.h>
#include <Driver_USBD.h>
#include <stddef.h>
#include <stdint.h>

static int sDebugToken = -1;

/// \def Size of endpoint description table: 4 2-byte words for maximum 8 endpoints
/// XXX: is it possible to use buffer table space, if not all EPs are present?
#define BUFFER_DESCRIPTOR_TABLE_SIZE (4 * 2 * 8)

/// \def Use "large" block size mapping RM0008 Rev 21 p 651
#define USBAD_USB_COUNTX_BLSIZE (1 << 15)

/// \def Use 64 bytes for buffer size
#define USBAD_USB_COUNTX_64_BYTES ((1 << 10) | USBAD_USB_COUNTX_BLSIZE)

#define USBAD_USB_BUFFER_SIZE (64)
#define USB_EPXR_STAT_RX_VALID (0b11)
#define USB_EPXR_EP_TYPE_CONTROL (0b11)
#define USBAD_USB_ISR_CONTEXT_FIFO_SIZE (10)
#define USBAD_USB_MAX_ENDPOINTS (1)

/// \enum Stores possible values for EPnR_STAT_<RX|TX> bits.
/// It does not matter whether "TX" or "RX" is being set,
/// as the values are the same.
enum UsbEpxrStatRxTx {
	UsbEpxrStatRxTxDisabled = 0b00,
	UsbEpxrStatRxTxStall    = 0b01,
	UsbEpxrStatRxTxNak      = 0b10,
	UsbEpxrStatRxTxValid    = 0b11,
};

struct I32Context {
	const char *title;
	uint32_t value;
} sI32Context[USBAD_USB_ISR_CONTEXT_FIFO_SIZE] = {{0}};

static void configureEndpointBdt(volatile USB_TypeDef *aUsb, uint8_t aEndpointNumber, uint8_t aBufferSize);
static void debugPrintUsbBdtContent(const void *aArg);
static void debugPrintI32Context(const void *aContext);

static Fifo sI32ContextFifo;;

static void debugPrintI32Context(const void *a)
{
	struct I32Context *usbIsrContext;
	(void)a;

	while ((usbIsrContext = fifoPop(&sI32ContextFifo)) != 0) {
		usvprintf("%s 0x%08X\r\n", usbIsrContext->title, usbIsrContext->value);
	}
}

static void debugEnqueueI32Context(const char *aTitle, uint32_t aValue)
{
	struct I32Context *context = fifoPush(&sI32ContextFifo);

	if (context) {
		*context = (struct I32Context) {
			.title = aTitle,
			.value = aValue,
		};
		usDebugAddTask(sDebugToken, debugPrintI32Context, 0);
	}
}

/// \details Handles low priority USB interrupts (RM0008 Rev 21 p 625)
void USB_LP_CAN1_RX0_IRQHandler()
{
	volatile USB_TypeDef *usb = USB;
	uint16_t istr = usb->ISTR;
	volatile uint16_t ep0r = usb->EP0R;
	uint32_t deviceEvent = 0;
	uint32_t endpointEvent = 0;
	uint8_t endpointNumber = 0;

    usb->ISTR &= ~(USB_ISTR_SOF | USB_ISTR_ESOF | USB_ISTR_ERR | USB_ISTR_PMAOVR);

	// Handle reset transaction as per RM0008 rev 21 p 639
	if (istr & USB_ISTR_RESET) {
        usb->ISTR &= ~(USB_ISTR_RESET | USB_ISTR_WKUP | USB_ISTR_SUSP);
        usb->CNTR &= ~(USB_CNTR_RESUME | USB_CNTR_FSUSP | USB_CNTR_LP_MODE | USB_CNTR_PDWN | USB_CNTR_FRES);

        // TODO
        setEpxrEpType(0, 1);
        setEpxrStatTx(0, 1);
        setEpxrStatRx(0, 1);

        usb->DADDR |= USB_DADDR_EF;

		return;
	}

	if (istr & USB_ISTR_CTR) {
		usb->ISTR &= ~(USB_ISTR_CTR);
    	usDebugPushMessage(sDebugToken, "Holy fuck!");

		return;
	}
}

static void debugPrintUsbBdtContent(const void *aArg)
{
	uint16_t usbBdtContent[64] = {0};
	usStm32f1UsbReadBdt((uint16_t *)&usbBdtContent, 64, 0);
	usvprintf("USB BDT content: ");
	usDebugPrintU16Array(usbBdtContent, 64);
	usvprintf("\r\n");
}

void usbInitialize()
{
	volatile USB_TypeDef *usb = USB;
	volatile RCC_TypeDef *rcc = RCC;

	rcc->APB1ENR |= RCC_APB1ENR_USBEN;
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
	NVIC_EnableIRQ(USBWakeUp_IRQn);

	// Keep in reset
	usb->CNTR = USB_CNTR_FRES;

	usb->ISTR = 0;
	usb->DADDR = 0;
	usb->BTABLE = 0;

	usStm32f1UsbSetBdt(0xffff, 64, 0);

	// Configure control endpoint BDT
	setUsbCountnRx(usb, 0, (1 << 15) | (1 << 10));
	setUsbCountnTx(usb, 0, 0);
	setUsbAddrnRx(usb, 0, 64);
	setUsbAddrnTx(usb, 0, 128);

	// Enable USB interrupts
	usb->CNTR =
		// Enable correct transfer interrupt
		USB_CNTR_CTRM
		// Enable reset interrupt
		| USB_CNTR_RESETM;

	// FIFO for debug info
	sDebugToken = usDebugRegisterToken("usb");
	fifoInitialize(&sI32ContextFifo, &sI32Context, USBAD_USB_ISR_CONTEXT_FIFO_SIZE, sizeof(struct I32Context));
	usDebugPushMessage(sDebugToken, "Initialization completed");
	usDebugAddTask(sDebugToken, debugPrintUsbBdtContent, 0);
}

#endif  // SRC_TARGET_STM32F103C6_SRC_USB_C_
