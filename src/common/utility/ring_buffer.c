//
// ring_buffer.c
//
// Created on: Oct 31, 2023
//     Author: Dmitry Murashov (d <DOT> murashov <AT> geoscan <DOT> aero)
//

#include "ring_buffer.h"

void ringBufferInitialize(RingBuffer *aRingBuffer)
{
	aRingBuffer->readPosition = 0;
	aRingBuffer->size = 0;
}

void ringBufferPutc(RingBuffer *aRingBuffer, unsigned char aCharacter)
{
	const unsigned writePosition = (aRingBuffer->readPosition + aRingBuffer->size) & (RING_BUFFER_MAX_SIZE - 1);  // mod <buffer size> if buffer size is exponent of 2
	aRingBuffer->buffer[writePosition] = aCharacter;

	if (aRingBuffer->size == RING_BUFFER_MAX_SIZE) {
		aRingBuffer->readPosition = (aRingBuffer->readPosition + 1) & (RING_BUFFER_MAX_SIZE - 1);
	} else {
		++aRingBuffer->size;
	}
}

int ringBufferTryGetc(RingBuffer *aRingBuffer, unsigned char *aOutCharacter)
{
	if (aRingBuffer->size) {
		*aOutCharacter = aRingBuffer->buffer[aRingBuffer->readPosition];
		aRingBuffer->readPosition = (aRingBuffer->readPosition + 1) & (RING_BUFFER_MAX_SIZE - 1);
		--aRingBuffer->size;

		return 1;
	}

	return 0;
}
