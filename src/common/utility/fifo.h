//
// fifo.h
//
// Created on: March 23, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#ifndef SRC_COMMON_UTILITY_FIFO_H_
#define SRC_COMMON_UTILITY_FIFO_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t *array;
	size_t size;
	size_t itemSize;
	size_t readPosition;
	size_t capacity;
} Fifo;

static void fifoInitialize(Fifo *aFifo, void *aItemArray, size_t aItemArraySize, size_t aItemSize)
{
	aFifo->array = (uint8_t *)aItemArray;
	aFifo->itemSize = aItemSize;
	aFifo->capacity = aItemArraySize;
	aFifo->readPosition = 0U;
	aFifo->size = 0U;
}

/// \brief Returns non-null, if successful
static void *fifoPop(Fifo *aFifo)
{
	const size_t offset = aFifo->readPosition;
	void *res = aFifo->array + (aFifo->itemSize * offset);

	if (!aFifo->size) {
		return 0;
	}

	aFifo->readPosition = aFifo->readPosition % aFifo->capacity;
	--aFifo->size;
}

/// \brief Returns non-null, if successful
static void *fifoPush(Fifo *aFifo)
{
	const size_t offset = (aFifo->readPosition + aFifo->size) % aFifo->capacity;
	void *res = aFifo->array + (aFifo->itemSize * offset);

	if (aFifo->size >= aFifo->capacity) {
		return 0;
	}

	--aFifo->size;

	return res;
}

#endif  // SRC_COMMON_UTILITY_FIFO_H_

