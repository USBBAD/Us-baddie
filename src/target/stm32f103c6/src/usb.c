//
// usb.c
//
// Created on: 15 February, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_STM32F103C6_SRC_USB_C_
#define SRC_TARGET_STM32F103C6_SRC_USB_C_

#include <stm32f103x6.h>

/// \def Size of endpoint description table: 4 2-byte words for maximum 8 endpoints
/// XXX: is it possible to use buffer table space, if not all EPs are present?
#define BUFFER_DESCRIPTOR_TABLE_SIZE (4 * 2 * 8)

/// \def Use "large" block size mapping RM0008 Rev 21 p 651
#define USBAD_USB_COUNTX_BLSIZE (1 << 15)

/// \def Use 64 bytes for buffer size
#define USBAD_USB_COUNTX_64_BYTES ((1 << 10) | USBAD_USB_COUNTX_BLSIZE)

#define USBAD_USB_BUFFER_SIZE (64)

/// \brief Memory layout for single buffer configuration
/// \details Memory mapping from APB to packet buffer memory is sparse,
/// because USB uses 16-bit words. Thus, 4 byte increment in MCU memory
/// corresponds to 2 byte increment in USB memory space. See also RM0008 Rev 21
/// p 628
typedef union {
    struct {
        volatile uint32_t addrTx;  // in units of uint16, always even
        volatile uint32_t countTx;
        volatile uint32_t addrRx;
        volatile uint32_t countRx;
    } btable[8];  // located here by virtue of USB.BTABLE being zero
    uint16_t buf[512]; // interspersed, 2 bytes data, 2 bytes reserved, only accessible as uint16 or uint32
} UsbMemoryMap;

static inline volatile UsbMemoryMap *getUsbMemoryMap()
{
	return (volatile UsbMemoryMap *)(&USB->BTABLE);
}

// TODO
void USB_HP_CAN1_TX_IRQHandler()
{
}

// TODO
void USB_LP_CAN1_RX0_IRQHandler()
{
}

// TODO
void USBWakeUp_IRQHandler()
{
}

static void enableClock()
{
	volatile RCC_TypeDef *rcc = RCC;
	rcc->APB1ENR |= RCC_APB1ENR_USBEN;
}

static void enableNvicInterrupts()
{
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
	NVIC_EnableIRQ(USBWakeUp_IRQn);
}

/// \brief Initialize control endpoint to enable enumeration.
/// \pre The device has not been enumerated yet, it has address 0, and only
/// has endpoint 0 being active
void initializeControlEndpoint(volatile USB_TypeDef *aUsb)
{
	aUsb->DADDR = 0;


	//  Enable transaction handling
	aUsb->DADDR |= USB_DADDR_EF;
}

static void initializeBufferDescriptionTable(volatile USB_TypeDef *aUsb)
{
	// Locate buffer table at 0th offset of the buffer table memory
	aUsb->BTABLE = 0;

	// TODO: XXX: endpoint-specific configuration
	// Control endpoint
	getUsbMemoryMap()->btable[0].addrTx = BUFFER_DESCRIPTOR_TABLE_SIZE;
	getUsbMemoryMap()->btable[0].addrRx = BUFFER_DESCRIPTOR_TABLE_SIZE + USBAD_USB_BUFFER_SIZE * 1;
	getUsbMemoryMap()->btable[0].countRx = USBAD_USB_COUNTX_64_BYTES;
}

static void enableUsbInterrupts(volatile USB_TypeDef *aUsb)
{
	// Enable correct transfer interrupt
	aUsb->CNTR |= USB_CNTR_CTRM;

	// Enable reset interrupt
	aUsb->CNTR |= USB_CNTR_RESETM;
}

static void onResetInterrupt()
{
	// TODO RM0008 Rev 21 p 626 "When a RESET interrupt is received, the
	// application software is responsible to enable again the default endpoint
	// of USB function 0 within 10mS from the end of reset sequence which
	// triggered the interrupt."
}

void usbInitialize()
{
	volatile USB_TypeDef *usb = USB;

	// RM0008 Rev 21 p 626
	enableClock();

	// Power up: deassert power down, RM0008 Rev 21 p 626
	// TODO XXX: the datasheet says it should be set, not reset. The reference
	// describes it as "switched off when 1". Plus, libcm3 resets the register.
	usb->CNTR &= ~(USB_CNTR_PDWN);

	// Wait for 1 uS TODO: replace w/ the actual delay XXX
	for (int i = 0xffffff; i; --i);

	// Deassert USB reset, RM0008 Rev 21 p 626
	usb->CNTR &= ~(USB_CNTR_FRES);

	// Clear any pending interrupts, RM0008 Rev 21 p 626
	usb->ISTR = 0;

	enableNvicInterrupts();
	// TODO configure other registers
	// TODO unmask USB events to enable interrupts

	initializeControlEndpoint(usb);

	initializeBufferDescriptionTable(usb);

	enableUsbInterrupts(usb);
}

#endif  // SRC_TARGET_STM32F103C6_SRC_USB_C_
