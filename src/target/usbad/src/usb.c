//
// usb.c
//
// Created on: 15 February, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_STM32F103C6_SRC_USB_C_
#define SRC_TARGET_STM32F103C6_SRC_USB_C_

#include "arm/stm32f1/stm32f1_usb.h"
#include "hal/usb.h"
#include "usb_control.h"
#include "utility/debug.h"
#include "utility/fifo.h"
#include "utility/ushelp.h"
#include "utility/usvprintf.h"
#include <stm32f103x6.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

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

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct I32Context {
	const char *title;
	uint32_t value;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void debugPrintUsbBdtContent(const void *aArg);
static void debugPrintI32Context(const void *aContext);
void USB_LP_CAN1_RX0_IRQHandler();

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct I32Context  sI32Context[USBAD_USB_ISR_CONTEXT_FIFO_SIZE] = {{0}};
static Fifo sI32ContextFifo;
static int sDebugToken = -1;
struct HalUsbDeviceDriver *sHalUsbDrivers[8] = {0};
uint16_t sTransactBuffer[USBAD_USB_BUFFER_SIZE] = {0};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

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
	uint32_t deviceEvent = 0;
	uint32_t endpointEvent = 0;
	uint8_t endpointNumber = 0;

    usb->ISTR &= ~(USB_ISTR_SOF | USB_ISTR_ESOF | USB_ISTR_ERR | USB_ISTR_PMAOVR);

	// Handle reset transaction as per RM0008 rev 21 p 639
	if (istr & USB_ISTR_RESET) {
        usb->ISTR &= ~(USB_ISTR_RESET | USB_ISTR_WKUP | USB_ISTR_SUSP);
        usb->CNTR &= ~(USB_CNTR_RESUME | USB_CNTR_FSUSP | USB_CNTR_LP_MODE | USB_CNTR_PDWN | USB_CNTR_FRES);

		initializeEp0UsbHalDeviceDriver();
        setEpxrEpType(0, 1);
        setEpxrStatTx(0, 1);
        setEpxrStatRx(0, 1);

        usb->DADDR |= USB_DADDR_EF;

		return;
	}

	if (istr & USB_ISTR_CTR) {
		usb->ISTR &= ~(USB_ISTR_CTR);

		uint8_t endpointId = (istr & USB_ISTR_EP_ID_Msk) >> USB_ISTR_EP_ID_Pos;
		uint16_t direction = (istr & USB_ISTR_DIR_Msk) >> USB_ISTR_DIR_Pos;
		uint16_t epxr = *getEpxr(endpointId);

		if (direction) {
			// If 1, OUT (/ SETUP?) transaction happened
			usStm32f1UsbReadBdt(&sTransactBuffer[0], USBAD_USB_BUFFER_SIZE, getEpxAddrnRxOffset(USBAD_USB_MAX_ENDPOINTS,
				USBAD_USB_BUFFER_SIZE, endpointId));
			// TODO: provide transaction type (SETUP, DATA0/1)
			union HalUsbDeviceContextVariant context = {
				.onRxIsr = {
					.endpointId = endpointId,
					.transactionFlags = (istr & USB_EP0R_SETUP ? HalUsbTransactionSetup : 0)
						| (istr & USB_ISTR_DIR ? HalUsbTransactionOut : HalUsbTransactionIn)
				},
			};

			if (context.onRxIsr.transactionFlags & HalUsbTransactionOut) {
				// Upon successful reception, hardware toggles corresponding data bit, so the value is inverted (unless double buffer is used)
				context.onRxIsr.transactionFlags |= (istr & USB_EP0R_DTOG_RX ? 0 : HalUsbTransactionData1);
			} else if (context.onRxIsr.transactionFlags & HalUsbTransactionIn) {
				// Upon successful reception, hardware toggles corresponding data bit, so the value is inverted (unless double buffer is used)
				context.onRxIsr.transactionFlags |= (istr & USB_EP0R_DTOG_TX ? 0 : HalUsbTransactionData1);
			}

			uint16_t nWordsu16;
			getUsbCountnRx(usb, endpointId, &nWordsu16);
			nWordsu16 &= (1 << 10) - 1;
			sHalUsbDrivers[endpointId]->onRxIsr(sHalUsbDrivers[endpointId], &context, sTransactBuffer,
				nWordsu16 * 2);
		}

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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

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
	setUsbAddrnRx(usb, 0, getEpxAddrnRxOffset(USBAD_USB_MAX_ENDPOINTS, USBAD_USB_BUFFER_SIZE, 0));
	setUsbAddrnTx(usb, 0, getEpxAddrnTxOffset(USBAD_USB_MAX_ENDPOINTS, USBAD_USB_BUFFER_SIZE, 0));

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

void halUsbDeviceRegisterDriver(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint)
{
	while (aEndpoint > 7 || !aDriver->onRxIsr) {
	}

	sHalUsbDrivers[aEndpoint] = aDriver;
}

#endif  // SRC_TARGET_STM32F103C6_SRC_USB_C_
