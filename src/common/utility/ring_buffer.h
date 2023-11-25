//
// ring_buffer.h
//
// Created on: Oct 31, 2023
//     Author: Dmitry Murashov (d <DOT> murashov <AT> geoscan <DOT> aero)
//

#ifndef COMMON_RING_BUFFER_H_
#define COMMON_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// \pre Must be power of 2
#define RING_BUFFER_MAX_SIZE (32)

typedef struct {
	unsigned char buffer[RING_BUFFER_MAX_SIZE];
	unsigned short readPosition;
	unsigned short size;
} RingBuffer;

void ringBufferInitialize(RingBuffer *aRingBuffer);
void ringBufferPutc(RingBuffer *aRingBuffer, unsigned char aCharacter);

/// \returns 0, if failed
int ringBufferTryGetc(RingBuffer *aRingBuffer, unsigned char *aOutCharacter);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // COMMON_RING_BUFFER_H_
