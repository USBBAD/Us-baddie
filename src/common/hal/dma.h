//
// dma.h
//
// Created on: Jan 28, 2024
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#ifndef COMMON_HAL_DMA_H_
#define COMMON_HAL_DMA_H_

/// \brief Returns a buffer for DMA (number, channel) pair
/// \pre The DMA peripheral must be initialized first
/// \returns 0, if the appropriate DMA hasn't been initialized
void *dmaGetBufferIsr(int aDma, int aDmaChannel);

void *dmaSetIsrHook(void (*aDmaHook)());

#endif // COMMON_HAL_DMA_H_
