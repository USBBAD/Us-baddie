//
// stm32f103c6_dma.c
//
// Created on: Jan 08, 2024
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include <stm32f103x6.h>
#include <stddef.h>
#include <stdint.h>

/// \def Buffer size for converted audio
#define USBAD_DMA1_CHANNEL1_BUFFER_SIZE_BYTES (16 * 2)

static uint8_t sDma1Channel1Buffer[USBAD_DMA1_CHANNEL1_BUFFER_SIZE_BYTES] = {0};
static void configureAudio();
static void (*sDma1Channel1IsrHook)();

void dma1Channel1Isr()
{
	// TODO: clear ISR (check if it is needed at all)
	// TODO: restart DMA (if not in circular mode)
	if (sDma1Channel1IsrHook) {
		sDma1Channel1IsrHook();
	}
}

/// \brief 2 audio channels are processed by DMA 1 which is wired to DMA
/// channel
///
/// \details
/// - 16 bit memory size
/// - 16 bit peripheral size
/// - Circular mode, auto memory increment (16 bit + 16 bit for each of 2 audio channels)
///
/// \ref See RM0008 Rev 21 p. 278 ("Channel configuration procedure")
static void configureAudio()
{
	volatile DMA_Channel_TypeDef *dmaChannel = DMA1_Channel1;
	// set max priority
	dmaChannel->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;

	// Set 16 bit memory size
	dmaChannel->CCR |= DMA_CCR_MSIZE_0;

	// Set 16 bit peripheral size
	dmaChannel->CCR |= DMA_CCR_PSIZE_0;

	// Enable automatic memory increment
	dmaChannel->CCR |= DMA_CCR_MINC;

	// Enable circular mode
	dmaChannel->CCR |= DMA_CCR_CIRC;

	// Set the number of transfers: 2, 16 bit per each audio channel
	dmaChannel->CNDTR = 2;

	// Set peripheral address: ADC1
	// TODO XXX: 16 bits, data alignment
	dmaChannel->CPAR = (uintptr_t)(&(ADC1->DR));

	// Set memory address (audio buffer stored values)
	// TODO XXX: check for possible memory alignment issues
	dmaChannel->CMAR = (uintptr_t)sDma1Channel1Buffer;

	// Enable channel
	dmaChannel->CCR |= DMA_CCR_EN
		// Enable inerrupt on "Transfer complete" event
		| DMA_CCR_TCIE;
}

void stm32f103c6DmaUp()
{
	configureAudio();
}

void *dmaGetBuffer(int aDma, int aDmaChannel)
{
	// Byte pack (dma, channel) -> (0x0000<u8_1><u8_2>)
	switch (aDma << 8 & aDmaChannel) {
		case (1 << 8) & 1:
			return sDma1Channel1Buffer;
	}

	return 0;
}
