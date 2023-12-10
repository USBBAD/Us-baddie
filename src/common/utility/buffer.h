//
// buffer.h
//
// Created on: Dec 10, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

#ifndef USBAD_UTILITY_BUFFER_H_
#define USBAD_UTILITY_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct ViewBuffer {
	uint8_t *storage;
	size_t maxSize;
	size_t currentSize;
};

struct ContinuousBufferIterator {
	uint8_t *current;
	uint8_t *end;
};

void bufferInitalizeViewBuffer(struct ViewBuffer *aBuffer, uint8_t *aStorage, size_t aMaxSize);
void bufferSetPayload(void *aBuffer, uint8_t *aPayload, size_t aPayloadLength);
size_t bufferGetPayloadSize(void *aBuffer);
void *bufferGetPayload(void *aBuffer);
void *bufferInitializeContinuousIterator(void *aBuffer, struct ContinuousBufferIterator *aContinuousIterator);
int bufferIteratorIsValid(void *aBufferIterator);

/// \pre `bufferIteratorIsValid(aBufferIterator)` returns true
int bufferIteratorTryGetNextByte(void *aBufferIterator, uint8_t *aOut);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // USBAD_UTILITY_BUFFER_H_
