//
// uart.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//
// Module-wide constraints:
// USART1 CR1 register is only set from thread mode

#include "clock.h"
#include "utility/ring_buffer.h"
#include <stm32f103x6.h>

#define USBAD_STM32F103C6_USARTDIV_COEFFICIENT (16.0f)
#define USBAD_STM32F103C6_BRR_FRACTION_NBITS (4)
#define USBAD_STM32F103C6_ENABLE_USART_1 (1)
#define USBAD_STM32F103C6_USART1_TRANSMISSION_ISR_BASED (1)

/// \brief USART TX interrupts that are used by the driver
#define USBAD_STM32F103C6_USART_TX_INTERRUPTS (USART_CR1_TXEIE | USART_CR1_TCIE)

#if USBAD_STM32F103C6_ENABLE_USART_1
static RingBuffer sUsart1TxRingBuffer;
static RingBuffer sUsart1RxRingBuffer;
#endif  // if USBAD_STM32F103C6_ENABLE_USART_1

// TODO: USARTs' ISRs

/// \brief Initializes RCC registers
static void configureClock();

/// \brief Input clock frequency for usart
static uint32_t getUsart1InputClockFrequency();

/// \brief see RM0008, rev. 21, p. 1136, "Fractional baudrate"
static uint32_t calculateBrrRegisterValue(uint32_t aBaudrate, uint32_t aInputFrequency);

static void enableAllUsartIsrs();

/// \brief Sets USART's "TX Empty interrupt enable" and other TX-related
/// interrupts flags
static void usartSetTxInterruptsEnabled(volatile USART_TypeDef *aUsart, int aIsEnabled);

static void usartSetTransmissionEnabled(volatile USART_TypeDef *aUsart, int aIsEnabled);

void usart1Isr()
{
	volatile USART_TypeDef *usart = USART1;
	volatile uint32_t sr = usart->SR;
	unsigned char nextCharacter = 0;

	if ((sr & (USART_SR_TXE | USART_SR_TC)) || ringBufferTryGetc(&sUsart1TxRingBuffer, &nextCharacter)) {
		usart->DR = nextCharacter;
	}
}

static void configureClock()
{
	volatile RCC_TypeDef *rcc = RCC;

	rcc->APB2ENR |= RCC_APB2ENR_USART1EN;
}

static uint32_t getUsart1InputClockFrequency()
{
	return clockGetSysclkFrequency() / clockGetApb2Prescaler();
}

static uint32_t calculateBrrRegisterValue(uint32_t aBaudrate, uint32_t aInputFrequency)
{
	const float usartDivf = (float)aInputFrequency  / (float)aBaudrate / USBAD_STM32F103C6_USARTDIV_COEFFICIENT;
	const uint32_t mantissa = (uint32_t)usartDivf;
	float fractionf = usartDivf - (float)mantissa;
	uint32_t fraction;
	fractionf *= USBAD_STM32F103C6_USARTDIV_COEFFICIENT;
	fraction = (uint32_t)fractionf;

	return (mantissa << USBAD_STM32F103C6_BRR_FRACTION_NBITS) | fraction;
}

static void enableAllUsartIsrs()
{
#if USBAD_STM32F103C6_ENABLE_USART_1 && USBAD_STM32F103C6_USART1_TRANSMISSION_ISR_BASED
	NVIC_EnableIRQ(USART1_IRQn);
#endif
}

static void usartSetTxInterruptsEnabled(volatile USART_TypeDef *aUsart, int aIsEnabled)
{
	if (aIsEnabled) {
		aUsart->CR1 |= USBAD_STM32F103C6_USART_TX_INTERRUPTS;
	} else {
		aUsart->CR1 &= ~USBAD_STM32F103C6_USART_TX_INTERRUPTS;
	}
}

static void usartSetTransmissionEnabled(volatile USART_TypeDef *aUsart, int aIsEnabled)
{
	if (aIsEnabled) {
		aUsart->CR1 |= USART_CR1_TE;
	} else {
		aUsart->CR1 &= ~USART_CR1_TE;
	}
}

void uartUp()
{
	configureClock();
}

int uartConfigure(uint8_t aUartNumber, uint32_t aBaudrate)
{
	uint32_t uartFrequency = 0;
	volatile USART_TypeDef *usart = 0;

	switch (aUartNumber) {

#if USBAD_STM32F103C6_ENABLE_USART_1
		case 1: {
			usart = USART1;
			uartFrequency = getUsart1InputClockFrequency();
			ringBufferInitialize(&sUsart1RxRingBuffer);
			ringBufferInitialize(&sUsart1TxRingBuffer);

			break;
		}
#endif

		default:
			return 0;
	}

	// Configure baudrate
	usart->BRR = calculateBrrRegisterValue(aBaudrate, uartFrequency);

	// Enable USART, "TX empty" interrupt, "RX not empty" interrupt
	usart->CR1 |= USART_CR1_UE | USART_CR1_TXEIE | USART_CR1_RXNEIE;

	enableAllUsartIsrs();
}

int uartTryPuts(uint8_t aUartNumber, const char *aString)
{
	int interruptNumber = 0;
	RingBuffer *txRingBuffer = 0;
	USART_TypeDef *usart = 0;

	switch (aUartNumber) {

#if USBAD_STM32F103C6_ENABLE_USART_1
		case 1:
			interruptNumber = USART1_IRQn;
			txRingBuffer = &sUsart1TxRingBuffer;
			usart = USART1;

			break;
#endif  // if USBAD_STM32F103C6_ENABLE_USART_1

		default:
			return 0;  // Unsupported UART
	}

	usartSetTxInterruptsEnabled(usart, 0);

	for (register const char *it = aString; *it != '\0'; ++it) {
		ringBufferPutc(txRingBuffer, *aString);
	}

	usartSetTxInterruptsEnabled(usart, 1);  // TODO: handle setting TXEIE from ISR
	usartSetTransmissionEnabled(usart, 1);

	return 1;
}
