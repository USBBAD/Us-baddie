//
// usb_isoch.c
//
// Created on: July 26, 2024
//     Author: Dmitry Murashov
//

#include "hal/usb.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void ep1OnRx(struct HalUsbDeviceDriver *, union HalUsbDeviceContextVariant *, const void *aBuffer, size_t aSize);
static void ep1OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext);

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct HalUsbDeviceDriver sep1UsbDriver = {
	.priv = 0,
	.onRxIsr = ep1OnRx,
	.onTxIsr = ep1OnTx,
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void ep1OnRx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext, const void *aBuffer,
	size_t aSize)
{
	(void)aDriver;
	(void)aContext;
	(void)aBuffer;
	(void)aSize;
}

static void ep1OnTx(struct HalUsbDeviceDriver *aDriver, union HalUsbDeviceContextVariant *aContext)
{
	(void)aDriver;
	(void)aContext;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void initializeEp1UsbHalDeviceDriver()
{
	halUsbDeviceRegisterDriver(&sep1UsbDriver, 1);
}
