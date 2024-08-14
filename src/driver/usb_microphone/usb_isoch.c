//
// usb_isoch.c
//
// Created on: July 26, 2024
//     Author: Dmitry Murashov
//


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define EP1_BUFFER_SIZE (64) /* TODO: duplicates descriptors_audio.h, and usb.c */
#if EP1_BUFFER_SIZE != 64
#error "Buffer sizes must be the same in descriptors_audio.s, and usb.c"
#endif /* EP1_BUFFER_SIZE != 64 */

#define USBAD_DEBUG_REGDUMP_FIFO_SIZE (4)

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/usb_microphone.h"
#include "hal/usb.h"
#include "utility/debug.h"
#include "utility/debug_regdump.h"
#include "utility/ushelp.h"
#include <stddef.h>
#include <stdint.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct Ep1DriverState {
	struct {
		const uint8_t *current;
		const uint8_t *end;
	} txState;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void ep1OnRx(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *, const void *aBuffer, size_t aSize);
static void ep1OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext);
static void advanceTxState(size_t aN);
static void setTxState(const uint8_t *aCurrent, size_t aN);
static int isTxStateFinished();
static size_t getTxStateRemainingBytes();
static void transmitBoundChecked();

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct HalUsbDeviceDriver sep1UsbDriver = {
	.priv = 0,
	.onRxIsr = ep1OnRx,
	.onTxIsr = ep1OnTx,
};

static struct Ep1DriverState sEp1DriverState = {
	.txState = {
		.current = 0,
		.end = 0,
	},
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern struct UsbMicrophoneHook *gUsbMicrophoneHook;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void ep1OnRx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext, const void *aBuffer,
	size_t aSize)
{
	/* Not used: this endpoint is "IN" endpoint */
	(void)aDriver;
	(void)aContext;
	(void)aBuffer;
	(void)aSize;
	usDebugPushMessage(0, "[isoch] EP1 on rx");
}

static void ep1OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext)
{
	transmitBoundChecked();
	if (isTxStateFinished() && gUsbMicrophoneHook) {
		gUsbMicrophoneHook->onChunkTransmitted();
	}
	usDebugPushMessage(0, "[isoch] successfully transmitted");
}

static inline void advanceTxState(size_t aN)
{
	sEp1DriverState.txState.current += aN;
}

static void setTxState(const uint8_t *aCurrent, size_t aN)
{
	sEp1DriverState.txState.current = aCurrent;
	sEp1DriverState.txState.end = aCurrent + aN;
}

static inline int isTxStateFinished()
{
	return sEp1DriverState.txState.current >= sEp1DriverState.txState.end;
}

static inline size_t getTxStateRemainingBytes()
{
	return sEp1DriverState.txState.end >= sEp1DriverState.txState.current ?
		sEp1DriverState.txState.end - sEp1DriverState.txState.current :
		0U;
}

static void transmitBoundChecked()
{
	const size_t bufferSize = US_MIN(getTxStateRemainingBytes(), EP1_BUFFER_SIZE);
	if (bufferSize) {
		debugRegdumpEnqueueI32Context("Sending # bytes:", (uint32_t)bufferSize);
		halUsbDeviceWriteTxIsr(&sep1UsbDriver, 1, sEp1DriverState.txState.current, bufferSize, 0);
		advanceTxState(bufferSize);
	} else {
		setTxState(0, 0); // Reset
	}
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void initializeEp1UsbHalDeviceDriver()
{
	debugRegdumpInitialize("");
	halUsbDeviceRegisterDriver(&sep1UsbDriver, 1);
}

void usbMicrophoneSetMonoPcm16Buffer(const uint16_t *aBuffer, size_t aSize)
{
	setTxState((const uint8_t *)aBuffer, aSize * 2);
	transmitBoundChecked();
}
