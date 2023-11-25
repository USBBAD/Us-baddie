#define OHDEBUG_PORT_ENABLE 1
#define OHDEBUG_TAGS_ENABLE "Trace"

#include <OhDebug.hpp>

#include "utility/ring_buffer.h"
#include <array>
#include <assert.h>
#include <stdint.h>

OHDEBUG_TEST("Test name")
{
	OHDEBUG("Trace", "Traced");
}

OHDEBUG_TEST("Ring buffer")
{
	RingBuffer ringBuffer;
	ringBufferInitialize(&ringBuffer);
	std::array<char, 5> array{"abdc"};

	for (auto c : array) {
		ringBufferPutc(&ringBuffer, static_cast<uint8_t>(c));
	}

	unsigned char c;
	size_t i = 0;

	while (ringBufferTryGetc(&ringBuffer, &c)) {
		assert(c == static_cast<char>(array[i++]));
	}
}

int main(void)
{
	OHDEBUG("Trace", "ring_buffer_test");
	OHDEBUG_RUN_TESTS();

	return 0;
}
