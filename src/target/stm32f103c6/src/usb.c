//
// usb.c
//
// Created on: 15 February, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_STM32F103C6_SRC_USB_C_
#define SRC_TARGET_STM32F103C6_SRC_USB_C_

#include "target/arm/stm32/stm32f1_usb.h"
#include "utility/debug.h"
#include "utility/fifo.h"
#include "utility/usvprintf.h"
#include <stm32f103x6.h>
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

#define USBAD_USB_ISR_CONTEXT_FIFO_SIZE (1)

#define USBAD_USB_MAX_ENDPOINTS (1)

/// \brief Memory layout for single buffer configuration
/// \details Memory mapping from APB to packet buffer memory is sparse,
/// because USB uses 16-bit words. Thus, 4 byte increment in MCU memory
/// corresponds to 2 byte increment in USB memory space. See also RM0008 Rev 21
/// p 628
typedef struct {
    struct {
        volatile uint32_t addrTx;  // in units of uint16, always even
        volatile uint32_t countTx;
        volatile uint32_t addrRx;
        volatile uint32_t countRx;
    } btable[8];  // located here by virtue of USB.BTABLE being zero
    uint16_t buf[512]; // interspersed, 2 bytes data, 2 bytes reserved, only accessible as uint16 or uint32
} UsbMemoryMap;

struct UsbIsrContext {
	uint16_t istr;
	uint16_t ep0r;
} sUsbIsrContext[USBAD_USB_ISR_CONTEXT_FIFO_SIZE] = {{0}};

static size_t getRxSize(uint8_t aEndpoint);

uint8_t sRxBuffer[64];
size_t sRxBufferSize;

static Fifo sUsbIsrContextFifo;

static inline volatile UsbMemoryMap *getUsbMemoryMap()
{
	return (volatile UsbMemoryMap *)(0x40006000);
}

/// \pre BTABLE register must be set
static inline void setUsbAddrnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void setUsbCountnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 2;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void setUsbAddrnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 4;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void setUsbCountnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 6;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbAddrnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbCountnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 2;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbAddrnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 4;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbCountnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 6;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \brief Get minimum required BDT offset based on the number of endpoints
/// The more the number of endpoints is, the bigger the table must be
/// RM0008 rev 21 p 650
static inline size_t getMinInnerBdtOffset(size_t aEndpoints)
{
	return aEndpoints * 8;
}

/// \details Handles high priority USB interrupts (RM0008 Rev 21 p 625)
void USB_HP_CAN1_TX_IRQHandler()
{
// TODO
	usDebugPushMessage(sDebugToken, "Got HP USB/CAN ISR");
}

static void dumpRegisters(const void *a)
{
	struct UsbIsrContext *usbIsrContext;
	(void)a;

	while ((usbIsrContext = fifoPop(&sUsbIsrContextFifo))) {
		if (usbIsrContext) {
			usvprintf("USB ISR registers\r\n", usbIsrContext->istr);
			usvprintf("ISTR=0x%08X\r\n", (uint32_t)usbIsrContext->istr);
			usvprintf("ISTR_CTR=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_CTR_Msk) >> USB_ISTR_CTR_Pos);
			usvprintf("ISTR_ERR=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_ERR_Msk) >> USB_ISTR_ERR_Pos);
			usvprintf("ISTR_WKUP=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_WKUP_Msk) >> USB_ISTR_WKUP_Pos);
			usvprintf("ISTR_SUSP=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_SUSP_Msk) >> USB_ISTR_SUSP_Pos);
			usvprintf("ISTR_RESET=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_RESET_Msk) >> USB_ISTR_RESET_Pos);
			usvprintf("ISTR_SOF=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_SOF_Msk) >> USB_ISTR_SOF_Pos);
			usvprintf("ISTR_ESOF=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_ESOF_Msk) >> USB_ISTR_ESOF_Pos);
			usvprintf("ISTR_DIR=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_DIR_Msk) >> USB_ISTR_DIR_Pos);
			usvprintf("ISTR_EP_ID=0x%08X\r\n", (usbIsrContext->istr & USB_ISTR_EP_ID_Msk) >> USB_ISTR_EP_ID_Pos);
			usvprintf("ISTR_EP0R=0x%08X\r\n", (usbIsrContext->ep0r));
			usvprintf("ISTR_EP0R_SETUP=0x%08X\r\n", (usbIsrContext->ep0r & USB_EP0R_SETUP_Msk) >> USB_EP0R_SETUP_Pos);
			usvprintf("ISTR_EP0R_CTR_RX=0x%08X\r\n", (usbIsrContext->ep0r & USB_EP0R_CTR_RX_Msk) >> USB_EP0R_CTR_RX_Pos);
		}
	}
}

static void printRxSize(const void *aSize)
{
	usvprintf("rx size: %d", (size_t)aSize);
}

static void enqueueDumpRegisters(uint16_t aIstr, uint16_t aEpxr)
{
	struct UsbIsrContext *usbIsrContext = fifoPush(&sUsbIsrContextFifo);
	*usbIsrContext = (struct UsbIsrContext) {
		.istr = aIstr,
		.ep0r = aEpxr,
	};
	usDebugAddTask(sDebugToken, dumpRegisters, 0);
}

/// \details Handles low priority USB interrupts (RM0008 Rev 21 p 625)
void USB_LP_CAN1_RX0_IRQHandler()
{
	volatile USB_TypeDef *usb = USB;
	// Load operation will clear the register, no need to assign to 0, RM0008 Rev 21 p 639
	volatile uint16_t istr = usb->ISTR;
	volatile uint16_t ep0r = usb->EP0R;
	size_t rxSize = getRxSize(0);

	if (rxSize) {
		usDebugAddTask(sDebugToken, printRxSize, (void *)rxSize);
		enqueueDumpRegisters(istr, ep0r);
	}

	usb->ISTR = 0;
}

void USBWakeUp_IRQHandler()
{
	usDebugPushMessage(sDebugToken, "Got USB wakeup ISR");
// TODO
}

static void enableClock()
{
	volatile RCC_TypeDef *rcc = RCC;
	rcc->APB1ENR |= RCC_APB1ENR_USBEN;
}

static void enableNvicInterrupts()
{
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
//	NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
//	NVIC_EnableIRQ(USBWakeUp_IRQn);
}

static size_t getRxSize(uint8_t aEndpoint)
{
	return (getUsbMemoryMap()->btable[aEndpoint].countRx & ((1 << 10) - 1));
}

/// \brief Initialize control endpoint to enable enumeration.
/// \pre The device has not been enumerated yet, it has address 0, and only
/// has endpoint 0 being active
void initializeControlEndpoint(volatile USB_TypeDef *aUsb)
{
	aUsb->EP0R = 0;
	// Switch expected DATA packet to "DATA1" (toggle, pre: hasn't been written before)
	aUsb->EP0R = USB_EP0R_DTOG_RX
		// Toggle STAT_RX bits to 0b11 to signify the endpoint as valid
		| 0b11 << USB_EP0R_STAT_RX_Pos
		// Doggle data packet into DATA1
		| USB_EP0R_DTOG_TX
		// Toggle STAT_TX bits to 0b11 to signify the endpoint as valid
		| 0b11 << USB_EP0R_STAT_TX_Pos;
}

void initializeEndpoints(volatile USB_TypeDef *aUsb)
{
	// Usb enumeration is yet to happen: set USB device address
	aUsb->DADDR = 0;

	initializeControlEndpoint(aUsb);
}

static void initializeBufferDescriptionTable(volatile USB_TypeDef *aUsb)
{
	// Locate buffer table at 0th offset of the buffer table memory
	aUsb->BTABLE = 0;

	setUsbAddrnTx(aUsb, 0, getMinInnerBdtOffset(USBAD_USB_MAX_ENDPOINTS));
	// Buffer size is 64, no double-buffering is used
	setUsbAddrnRx(aUsb, 0, getMinInnerBdtOffset(USBAD_USB_MAX_ENDPOINTS) + USBAD_USB_BUFFER_SIZE);

	// BL_SIZE = 1 | NUM_BLOCK = 1 (gets interpreted as 64 bytes): no double buffering is used, RM0008 rev 21 p 650
#if USBAD_USB_BUFFER_SIZE != 64
#  error Incorrect block configuration!
#endif
	setUsbCountnRx(aUsb, 0, (1 << 15) | (1 << 10));

	// Not needed, just for symmetry
	setUsbCountnTx(aUsb, 0, 0);
}

static void enableUsbInterrupts(volatile USB_TypeDef *aUsb)
{
	// Enable correct transfer interrupt
	aUsb->CNTR |= USB_CNTR_CTRM
		// Enable reset interrupt
		| USB_CNTR_RESETM
		// Enable start of frame interrupt
		| USB_CNTR_SOFM;
}

static void onResetInterrupt()
{
	// TODO RM0008 Rev 21 p 626 "When a RESET interrupt is received, the
	// application software is responsible to enable again the default endpoint
	// of USB function 0 within 10mS from the end of reset sequence which
	// triggered the interrupt."
}

static void enableUsbDevice(volatile USB_TypeDef *aUsb)
{
	aUsb->DADDR |= USB_DADDR_EF;
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

	// RM0008 Rev 21 p 626
	enableClock();

	// Power up: deassert power down, RM0008 Rev 21 p 626
	// TODO XXX: the datasheet says it should be set, not reset. The reference
	// describes it as "switched off when 1". Plus, libcm3 resets the register.
	usb->CNTR = 0;

	// Wait for 1 uS TODO: replace w/ the actual delay XXX
	for (int i = 0xffffff; i; --i);

	// Clear any pending interrupts, RM0008 Rev 21 p 626
	usb->ISTR = 0;

	// Reset other registers XXX
	usb->DADDR = 0;
	usb->BTABLE = 0;

	enableNvicInterrupts();
	// TODO configure other registers
	// TODO unmask USB events to enable interrupts

	initializeEndpoints(usb);
	initializeBufferDescriptionTable(usb);
	enableUsbInterrupts(usb);
	enableUsbDevice(usb);
	sDebugToken = usDebugRegisterToken("usb");
	fifoInitialize(&sUsbIsrContextFifo, &sUsbIsrContext, USBAD_USB_ISR_CONTEXT_FIFO_SIZE, sizeof(struct UsbIsrContext));
	usDebugPushMessage(sDebugToken, "Initialization completed");
	usDebugAddTask(sDebugToken, debugPrintUsbBdtContent, 0);
}

#endif  // SRC_TARGET_STM32F103C6_SRC_USB_C_
