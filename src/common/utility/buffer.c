//
// buffer.c
//
// Created on: Dec 10, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#include "buffer.h"
#include "chelp.h"

void initalizeViewBuffer(struct ViewBuffer *aBuffer, uint8_t *aStorage, size_t aMaxSize)
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
