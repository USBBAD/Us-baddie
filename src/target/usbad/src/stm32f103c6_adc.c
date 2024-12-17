//
// stm32f103c6_adc.c
//
// Created on: Jan 08, 2024
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define USBAD_ADC_SMPR (55) /**< \def Offset for ADC SMPR, number of cycles to skip for single sample */

/****************************************************************************
 * Included files
 ****************************************************************************/

#include <stm32f103x6.h>

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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void adc12Isr()
{
}

void stm32f103c6AdcUp()
{
	NVIC_EnableIRQ(ADC1_2_IRQn);
	volatile ADC_TypeDef *adc = ADC1;

	// Enable EOC (end of conversion) interrupt
//	adc->CR1 |= ADC_CR1_EOCIE;

	// Enable SCAN mode (scan each of the enabled channels, one by one)
	adc->CR1 |= ADC_CR1_SCAN;

	// Enable DMA
	adc->CR2 |= ADC_CR2_DMA;

	// Enable continuous mode (re-enable conversion after it is finished)
	adc->CR2 |= ADC_CR2_CONT;

	/* Set offset for the number of sampling cycles
	Sampling frequency = SYSCLK / (AHBPRE * APB2PRE * ADCPRE * (12.5 + (USBAD_ADC_SMPR + 0.5))) */
#if USBAD_ADC_SMPR == 55
	adc->SMPR2 |= 0b101 << ADC_SMPR2_SMP6_Pos;
#else
	error "Unsupported configuration for ADC SMPR"
#endif

	// Configure conversion sequence

	adc->SMPR2 |= (0b011 << ADC_SMPR2_SMP0_Pos);

	// "b0000": 1 conversion
	adc->SQR1 |= 0 << 20;

	// Use ADC12_IN1 (PA1) as first conversion
	adc->SQR3 |= 1;

	// Start conversion
	adc->CR2 |= ADC_CR2_SWSTART;

	// Enable ADC
	adc->CR2 |= ADC_CR2_ADON;

	// Calibrate ADC
	adc->CR2 |= ADC_CR2_CAL;

	while (adc->CR2 & ADC_CR2_CAL);

	// Start conversion
//	adc->CR2 |= ADC_CR2_ADON;
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
