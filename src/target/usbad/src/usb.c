//
// usb.c
//
// Created on: 15 February, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_STM32F103C6_SRC_USB_C_
#define SRC_TARGET_STM32F103C6_SRC_USB_C_

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS (2)
#define USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE (64)
#define USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE (0)
#define USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE_TX (64)

#define USBAD_USB_BUFFER_SIZE USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE
#define USBAD_USB_MAX_ENDPOINTS USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS

/**
 * @def Configuration for "debug_regdump.h"
 */
#define USBAD_DEBUG_REGDUMP_FIFO_SIZE (4)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "arm/stm32f1/stm32f1_usb.h"
#include "arm/stm32f1/stm32f1_usb_bdt_layout.h"
#include "hal/usb.h"
#include "utility/debug.h"
#include "utility/debug_regdump.h"
#include "utility/fifo.h"
#include "utility/ushelp.h"
#include "utility/usvprintf.h"
#include <stm32f103x6.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

enum StmUsbdState {
	StmUsbdStateDisabled = 0b00,
	StmUsbdStateValid = 0b11,
	StmUsbdStateNak = 0b10,
	StmUsbdStateStall = 0b01,
};

enum StmUsbdEpType {
	StmUsbdEpTypeBulk = 0,
	StmUsbdEpTypeControl = 1,
	StmUsbdEpTypeIso = 2,
	StmUsbdEpTypeInterrupt = 3,
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void debugPrintUsbBdtContent(const void *aArg);
void USB_LP_CAN1_RX0_IRQHandler();

/****************************************************************************
 * Private Data
 ****************************************************************************/

static int sDebugToken = -1;
static struct HalUsbDeviceDriver *sHalUsbDrivers[8] = {0};
static uint16_t sTransactBuffer[128] = {0};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * @brief USB_LP_CAN1_RX0_IRQHandler
 * @details Handles low priority USB interrupts (RM0008 Rev 21 p 625)

 */
void USB_LP_CAN1_RX0_IRQHandler()
{
	volatile USB_TypeDef *usb = USB;
	const uint16_t istr = usb->ISTR;
	uint32_t deviceEvent = 0;
	uint32_t endpointEvent = 0;
	const uint8_t endpointId = (istr & USB_ISTR_EP_ID_Msk) >> USB_ISTR_EP_ID_Pos;
	const uint16_t istrPmaOverrun = istr & USB_ISTR_PMAOVR;
	const uint16_t istrEsof = istr & USB_ISTR_ESOF;
	const uint16_t istrErr = istr & USB_ISTR_ERR;

	/* Handle errors */
	if (istrPmaOverrun) {
		usDebugPushMessage(0, "![usb] PMAOVR");
	}
	if (istrEsof) {
		usDebugPushMessage(0, "![usb] ESOF");
	}
	if (istrErr) {
		usDebugPushMessage(0, "![usb] ERR");
	}

	if (endpointId != 0) {
		debugRegdumpEnqueueI32Context("ISR for EP #", endpointId);
	}

    usb->ISTR &= ~(USB_ISTR_SOF | USB_ISTR_ESOF | USB_ISTR_ERR | USB_ISTR_PMAOVR);
    usb->CNTR |= USB_CNTR_SOFM;

	// Handle reset transaction as per RM0008 rev 21 p 639
	if (istr & USB_ISTR_RESET) {
        usb->ISTR &= ~(USB_ISTR_RESET | USB_ISTR_WKUP | USB_ISTR_SUSP);
        usb->CNTR &= ~(USB_CNTR_RESUME | USB_CNTR_FSUSP | USB_CNTR_LP_MODE | USB_CNTR_PDWN | USB_CNTR_FRES);

		/* EP 0 */
		setEpxrEpType(0, StmUsbdEpTypeControl);
		setEpxrDtogRx(0, 0);
		setEpxrDtogTx(0, 0);
		setEpxrStatTx(0, StmUsbdStateNak);
		setEpxrStatRx(0, StmUsbdStateNak);

		/* EP 1 */
		gUsbBdt->bdt[1].countTx = USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE_TX;
		setEpxrEpType(1, StmUsbdEpTypeIso); /* ISOCH */
		setEpxrDtogTx(1, 0);
		setEpxrEa(1, 1);
		/* The only valid states for ISOCH EPs are "Disabled", or "Valid" */
		setEpxrStatRx(1, StmUsbdStateDisabled);
		setEpxrStatTx(1, StmUsbdStateValid);

        usb->DADDR |= USB_DADDR_EF;

		return;
	}

	if (istr & USB_ISTR_CTR) {
		uint16_t epxr = *getEpxr(endpointId);

		if (endpointId == 1) {
			usDebugPushMessage(0, "EP 1 ISR");
		}

		usb->ISTR &= ~(USB_ISTR_CTR);

		if (epxr & USB_EP0R_CTR_RX) {
			uint16_t nWordsu16 = gUsbBdt->bdt[0].countRx & ((1 << 10) - 1);
			// Reset ISR flag
			resetEpxrCtrRx(endpointId);

			// Handle OUT or SETUP transactions
			union HalUsbDeviceContextVariant context = {
				.onRxIsr = {
					.endpointId = endpointId,
					.transactionFlags = 0,
				},
			};

			// Read out buffer, and its length
			usStm32f1UsbReadBdt(&sTransactBuffer[0], nWordsu16, getEpxAddrnRxOffset(endpointId));

			// Initialize context
			if (epxr & USB_EP0R_SETUP) {
				context.onRxIsr.transactionFlags |= HalUsbTransactionSetup;
			} else {
				context.onRxIsr.transactionFlags |= HalUsbTransactionOut;
			}
			// Upon successful reception, hardware toggles corresponding data bit, so the value is inverted (unless double buffer is used)
			context.onRxIsr.transactionFlags |= (epxr & USB_EP0R_DTOG_RX ? 0 : HalUsbTransactionData1);

			// Pass further handling to the driver
			sHalUsbDrivers[endpointId]->onRxIsr(sHalUsbDrivers[endpointId], &context, sTransactBuffer,
				nWordsu16 * 2);

			setEpxrStatRx(0, 0b11); // Set STAT_RX valid (TODO: daedalean's version was setting STAT_TX)

		} else if (epxr & USB_EP0R_CTR_TX) {
			resetEpxrCtrTx(endpointId);
			// Handle IN transactions
			union HalUsbDeviceContextVariant context = {
				.onTxIsr = {
					.endpointId = endpointId,
					.transactionFlags = 0,
				},
			};
			context.onTxIsr.transactionFlags |= HalUsbTransactionIn;
			// Upon successful reception, hardware toggles corresponding data bit, so the value is inverted (unless double buffer is used)
			context.onTxIsr.transactionFlags |= (epxr & USB_EP0R_DTOG_TX ? 0 : HalUsbTransactionData1);

			sHalUsbDrivers[endpointId]->onTxIsr(sHalUsbDrivers[endpointId], &context);
		}

		return;
	}
}

static void debugPrintUsbBdtContent(const void *aArg)
{
	uint16_t usbBdtContent[16] = {0};
	uint32_t offset = (uint32_t)aArg;
	usStm32f1UsbReadBdt((uint16_t *)&usbBdtContent, US_ARRAY_SIZE(usbBdtContent), offset);
	usvprintf("USB BDT content at offset ");
	usDebugPrintHex32(offset);
	usvprintf("");
	usDebugPrintU16Array(usbBdtContent, US_ARRAY_SIZE(usbBdtContent));
	usvprintf("\r\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbInitialize()
{
	// FIFO for debug info
	debugRegdumpInitialize("usb");

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

	usStm32f1UsbSetBdt(0, 256, 0);

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 2
#error "STM32 buffer size setting is only impemented for 2 endpoints. Implement the other ones."
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS != 2 */

	// Configure control endpoint BDT
#if USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE == 64
	setUsbCountnRx(usb, 0, (1 << 15) | (1 << 10));
#else /* USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE == 64 */
#error "The buffer size has to be adjusted accordingly"
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE == 64 */
	setUsbCountnTx(usb, 0, 0);

	// Configure isoch endpoint BDT
#if USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE_TX == 64
	setUsbCountnRx(usb, 1, (1 << 15) | (1 << 10));
#else /* USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE == 64 */
#error "The buffer size has to be adjusted accordingly"
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE == 64 */

	/* Assign address to each EP */
	for (int i = 0; i < 8; ++i) {
		setEpxrEa(i, i);
		setUsbAddrnTx(usb, i, getEpxAddrnTxOffset(i));
		setUsbAddrnRx(usb, i, getEpxAddrnRxOffset(i));
		setUsbCountnTx(usb, i, 0);
	}

	// Enable USB interrupts
	usb->CNTR =
		// Enable correct transfer interrupt
		USB_CNTR_CTRM
		// Enable reset interrupt
		| USB_CNTR_RESETM
		| USB_CNTR_SOFM;

	usDebugPushMessage(getDebugToken(), "Initialization completed");
	usDebugAddTask(getDebugToken(), debugPrintUsbBdtContent, 0);
}

void halUsbDeviceRegisterDriver(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint)
{
	// Debug trap
	while (aEndpoint > 7 || !aDriver->onRxIsr) {
	}

	sHalUsbDrivers[aEndpoint] = aDriver;
}

void halUsbDeviceWriteTxIsr(struct HalUsbDeviceDriver *aDriver, uint8_t aEndpoint, const void *aBuffer, size_t aSize,
	int aIsData1)
{
	// Copy data into USB buffer
	volatile uint32_t *out = getTxBufferAhb(aEndpoint);
	if (!out) {
		usDebugPushMessage(0, "![usb] Couldn't get EP buffer");
		return;
	}
	size_t remaining = aSize / 2;
	for (const uint16_t *in = aBuffer; remaining; --remaining) {
		*out = *in;
		++out;
		++in;
	}
	if (aSize % 2) {
		*out = ((const uint8_t *)aBuffer)[aSize - 1];
	}

#if 0
	if (aEndpoint == 1) {
		usDebugAddTask(getDebugToken(), debugPrintUsbBdtContent, (const void *)192);
	}
#endif

	// Set counter
	gUsbBdt->bdt[aEndpoint].countTx = aSize;

#warning Target-specific code. Double buffer is used
	if (aEndpoint == 1) {
		gUsbBdt->bdt[1].countRx = aSize;
		setEpxrStatRx(aEndpoint, StmUsbdStateValid);
	}


	// DATAx, set the correct type
	setEpxrDtogTx(aEndpoint, aIsData1 ? 1 : 0);

	// Enable TX transaction
	setEpxrStatTx(aEndpoint, StmUsbdStateValid);

#if 0 /* Debug: dump content */
	if aBuffer(aEndpoint == 1) {
		usDebugAddTask(getDebugToken(), debugPrintUsbBdtContent, (const void *)192);
		usDebugAddTask(getDebugToken(), debugPrintUsbBdtContent, 0);
	}
#endif
}

void halUsbDeviceSetAddress(struct HalUsbDeviceDriver *aDriver, uint8_t aAddress)
{
	USB->DADDR |= aAddress & ((1 << 7) - 1);
}

void halUsbSetEpState(struct HalUsbDeviceDriver *aDriver, uint8_t aEp, enum HalUsbEpState aEpStateIn,
	enum HalUsbEpState aEpStateOut)
{
	static const uint8_t stateMap[] = {
		[HalUsbEpStateDisabled] = StmUsbdStateDisabled,
		[HalUsbEpStateValid] = StmUsbdStateValid,
		[HalUsbEpStateNak] = StmUsbdStateNak,
		[HalUsbEpStateStall] = StmUsbdStateStall,
	};
	setEpxrStatRx(aEp, stateMap[aEpStateOut]);
	setEpxrStatTx(aEp, stateMap[aEpStateIn]);
}

#endif  // SRC_TARGET_STM32F103C6_SRC_USB_C_
