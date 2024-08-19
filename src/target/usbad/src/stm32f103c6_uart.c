//
// uart.c
//
// Created on: Oct 28, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//
// Module-wide constraints:
// USART1 CR1 register is only set from thread mode

#include "clock.h"
#include <stm32f103x6.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define USBAD_STM32F103C6_USARTDIV_COEFFICIENT (16.0f)
#define USBAD_STM32F103C6_BRR_FRACTION_NBITS (4)
#define USBAD_STM32F103C6_ENABLE_USART_1 (1)
#define USBAD_STM32F103C6_USART1_TRANSMISSION_ISR_BASED (1)

/// \brief USART TX interrupts that are used by the driver
#define USBAD_STM32F103C6_USART_TX_INTERRUPTS (USART_CR1_TXEIE | USART_CR1_TCIE)
#define USBAD_USART1_TX_BUFFER_SIZE (64)

struct Tx {
	uint8_t *buffer;
	const uint8_t *it;
	const uint8_t *itEnd;
	size_t size;
};

static int txIsTransmitting(const struct Tx *aTx)
{
	return aTx->it != 0;
}

static int txInit(struct Tx *aTx, const char *aBuffer, size_t aBufferSize)
{
	*aTx = (struct Tx) {
		.buffer = (uint8_t *)aBuffer,
		.it = (uint8_t *)aBuffer,
		.itEnd = 0,
		.size = aBufferSize,
	};
}

static int txPutStringLen(struct Tx *aTx, const char *aBuffer, size_t bufferLen)
{
	if (bufferLen > aTx->size) {
		return 0;
	}

	memcpy(aTx->buffer, aBuffer, strlen(aBuffer));
	aTx->it = aTx->buffer;
	aTx->itEnd = aTx->it + bufferLen;

	return 1;
}

static int txPutString(struct Tx *aTx, const char *aBuffer)
{
	size_t bufferLength = strlen(aBuffer);
	txPutStringLen(aTx, aBuffer, bufferLength);
}

static const uint8_t *txTryPop(struct Tx *aTx)
{
	const uint8_t *result = 0;

	if (aTx->it < aTx->itEnd) {
		result = aTx->it;
		++aTx->it;
	} else {
		aTx->it = 0;
		aTx->itEnd = 0;
	}

	return result;
}

#if USBAD_STM32F103C6_ENABLE_USART_1
static char sUart1TxBuffer[USBAD_USART1_TX_BUFFER_SIZE] = {0};
static struct Tx sUart1Tx;
#endif  // if USBAD_STM32F103C6_ENABLE_USART_1

// TODO: USARTs' ISRs

/// \brief Initializes RCC registers

static void configureGpio();

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

	if (sr & (USART_SR_TXE | USART_SR_TC)) {
		const uint8_t *ch = txTryPop(&sUart1Tx);

		if (ch) {
			usart->DR = *ch;
		} else {  // TX buffer is empty
			usartSetTxInterruptsEnabled(usart, 0);
		}
	}
}

static void configureGpio()
{
#if USBAD_STM32F103C6_ENABLE_USART_1
	{
		volatile GPIO_TypeDef *gpio = GPIOA;

		// Configure TX pin (PA9),  RM0008 rev 21 p 166
		gpio->CRH |= GPIO_CRH_CNF9_1  // Alternate function push-pull
			| GPIO_CRH_MODE9_1;  // Mode output, 2 MHz (USART won't require much)

		// Configure RX pin (PA10), input mode pull up
		gpio->CRH |= GPIO_CRH_MODE10_1;  // Input with PU / PD (we need "pull up")
		gpio->ODR |= GPIO_ODR_ODR10;  // Pull up (RM0008 rev 21 p 161)
	}
#endif //  USBAD_STM32F103C6_ENABLE_USART_1
}

static uint32_t getUsart1InputClockFrequency()
{
	return clockGetSysclkFrequency() / clockGetApb2Prescaler() / clockGetAhbPrescaler();
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
	NVIC_SetPriority(USART1_IRQn, 1);
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
	configureGpio();

	// Set the lowest priority for UART
	NVIC_SetPriority(USART1_IRQn, 255);
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

#if USBAD_STM32F103C6_ENABLE_USART_1
			txInit(&sUart1Tx, sUart1TxBuffer, USBAD_USART1_TX_BUFFER_SIZE);
#endif  // USBAD_STM32F103C6_ENABLE_USART_1

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

int uartTryPutsLen(uint8_t aUartNumber, const char *aString, size_t aBufferLength)
{
	int interruptNumber = 0;
	void *txBuffer = 0;
	volatile USART_TypeDef *usart = 0;
	struct Tx *tx;
	int result = 1;

	switch (aUartNumber) {

#if USBAD_STM32F103C6_ENABLE_USART_1
		case 1:
			interruptNumber = USART1_IRQn;
			usart = USART1;
			tx = &sUart1Tx;

			break;
#endif  // if USBAD_STM32F103C6_ENABLE_USART_1

		default:
			return 0;  // Unsupported UART
	}

	usartSetTxInterruptsEnabled(usart, 0);

	if (txIsTransmitting(tx)) {
		result = 0;
	} else if (!txPutStringLen(tx, aString, aBufferLength)) {
		result = 0;
	}

	usartSetTxInterruptsEnabled(usart, 1);  // TODO: handle setting TXEIE from ISR
	usartSetTransmissionEnabled(usart, 1);

	return result;
}

int uartTryPuts(uint8_t aUartNumber, const char *aOutput)
{
	size_t bufferLen = strlen(aOutput);

	return uartTryPutsLen(aUartNumber, aOutput, bufferLen);
}

int uartBusyWaitForWritten(uint8_t aUartNumber)
{
	struct Tx *tx = 0;

	switch (aUartNumber) {
		case 1:
			tx = &sUart1Tx;

			break;
	}

	if (tx) {
		while (txIsTransmitting(tx));
	}
}
