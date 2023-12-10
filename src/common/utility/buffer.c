//
// buffer.c
//
// Created on: Dec 10, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include "buffer.h"
#include "chelp.h"

void bufferInitalizeViewBuffer(struct ViewBuffer *aBuffer, uint8_t *aStorage, size_t aMaxSize)
{
	aBuffer->storage = aStorage;
	aBuffer->maxSize = aMaxSize;
	aBuffer->currentSize = 0;
}

void bufferSetPayload(void *aBuffer, uint8_t *aPayload, size_t aPayloadLength)
{
	((struct ViewBuffer *)aBuffer)->currentSize = USBAD_MIN(aPayloadLength, ((struct ViewBuffer *)aBuffer)->maxSize);
}

size_t bufferGetPayloadSize(void *aBuffer)
{
	return ((struct ViewBuffer *)aBuffer)->currentSize;
}

void *bufferGetPayload(void *aBuffer)
{
	return ((struct ViewBuffer *)aBuffer)->storage;
}

void *bufferInitializeContinuousIterator(void *aBuffer, struct ContinuousBufferIterator *aContinuousIterator)
{
	aContinuousIterator->current = ((struct ViewBuffer *)aBuffer)->storage;
	aContinuousIterator->end = ((struct ViewBuffer *)aBuffer)->storage + ((struct ViewBuffer *)aBuffer)->currentSize;
}

int bufferIteratorIsValid(void *aBufferIterator)
{
	return ((struct ContinuousBufferIterator *)aBufferIterator)->current != 0;
}

int bufferIteratorTryGetNextByte(void *aBufferIterator, uint8_t *aOut)
{
	if (((struct ContinuousBufferIterator *)aBufferIterator)->current == ((struct ContinuousBufferIterator *)aBufferIterator)->end) {
		return 0;
	}

	*aOut = *((struct ContinuousBufferIterator *)aBufferIterator)->current;
	++((struct ContinuousBufferIterator *)aBufferIterator)->current;

	return 1;
}
