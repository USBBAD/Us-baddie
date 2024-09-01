//
// stm32f103c6_adc.c
//
// Created on: Jan 08, 2024
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "stm32f103c6_dma.h"
#include "us_assert.h"
#include "us_error.h"
#include <stm32f103x6.h>
#include <stddef.h>
#include <stdint.h>

extern void stm32f103c6DmaUpAdc(uint16_t *aDmaBuf, size_t aDmaBufSize, void(*aOnDmaFinishedHook)(),
	volatile ADC_TypeDef *aAdc);

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void stm32f103c6AdcUp(volatile ADC_TypeDef *aAdc, const uint8_t *aChannels, size_t aChannelsSize)
{
	/* TODO: adjust ADC frequency, to match w/ USB speed, so a complete 64-byte frame is sent on each SOF */
	US_ASSERT(aChannels);

	NVIC_EnableIRQ(ADC1_2_IRQn);

	/* Enable EOC (end of conversion) interrupt */
//	aAdc->CR1 |= ADC_CR1_EOCIE;
	/* Enable SCAN mode (scan each of the enabled channels, one by one) */
	aAdc->CR1 |= ADC_CR1_SCAN;
	/* Enable DMA */
	aAdc->CR2 |= ADC_CR2_DMA;
	/* Enable continuous mode (re-enable conversion after it is finished) */
	aAdc->CR2 |= ADC_CR2_CONT;

	/* Configure conversion sequence */
	/* 2 conversions */
	aAdc->SQR1 |= aChannelsSize;
	for (size_t i = 0; i < aChannelsSize; ++i) {
		// Figure out the right register, and offset
		size_t nsqr = i / 6;
		uint32_t offset = i % 5;
		uint32_t channel = aChannels[i];
		(&aAdc->SQR1)[nsqr] |= channel << offset;
	}

	/* Start conversion */
	aAdc->CR2 |= ADC_CR2_SWSTART;

	/* Enable ADC */
	aAdc->CR2 |= ADC_CR2_ADON;
	/* Calibrate ADC */
	aAdc->CR2 |= ADC_CR2_CAL;

	while (aAdc->CR2 & ADC_CR2_CAL);

	/* Start conversion */
//	adc->CR2 |= ADC_CR2_ADON;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void adc12Isr()
{
}

void adcStart()
{
	// Start conversion
	volatile ADC_TypeDef *adc = ADC1;
	volatile DMA_Channel_TypeDef *dmaChannel = DMA1_Channel1;
	adc->CR2 |= ADC_CR2_ADON;
	dmaChannel->CCR |= DMA_CCR_EN;
}

void adcStopIsr()
{
	// Start conversion
	volatile ADC_TypeDef *adc = ADC1;
	volatile DMA_Channel_TypeDef *dmaChannel = DMA1_Channel1;
	adc->CR2 &= ~(ADC_CR2_ADON);
	dmaChannel->CCR &= ~(DMA_CCR_EN);
}
