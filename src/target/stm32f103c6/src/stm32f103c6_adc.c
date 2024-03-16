//
// stm32f103c6_adc.c
//
// Created on: Jan 08, 2024
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include <stm32f103x6.h>

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

	// Configure conversion sequence

	// 2 conversions
	adc->SQR1 |= ADC_SQR1_L_0;

	// Use ADC12_IN1 (PA1) as first conversion
	adc->SQR3 |= 1;

	// Use ADC12_IN2 (PA2) as second conversion
	adc->SQR3 |= 2 << 5;

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
	adc->CR2 |= ADC_CR2_ADON;
}
