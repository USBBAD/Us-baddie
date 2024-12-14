/**
 * stat.c
 *
 * Created on: December 08, 2024
 *     Author: Dmitry Murashov
 */


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Included files
 ****************************************************************************/

#include "driver/usb_microphone/usb_microphone.h"
#include "system/time.h"
#include "utility/debug.h"
#include <string.h>

#include "stat.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

struct Stat gSysStat;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void sysStatPrint()
{
	usvprintf("--- uptime [s]: ");
	usDebugPrintHex32(timeGetUptimeUs() / 1000000);
	usvprintf("\r\n");
	usvprintf("ISOCH:      ");
	usDebugPrintHex32(gSysStat.usbIsochPackets);
	usvprintf("\r\n");
	usvprintf("mic. en.:   ");
	usDebugPrintHex8(usbMicrophoneIsEnabled());
	usvprintf("\r\n");
	usvprintf("USB errors: ");
	usDebugPrintHex32(gSysStat.usbErr);
	usvprintf("\r\n");
	usvprintf("ISOCH[B]:   ");
	usDebugPrintHex32(gSysStat.usbIsochB);
	usvprintf("\r\n");
	usvprintf("audio mean: ");
	usDebugPrintHex16(gSysStat.audioMean);
	usvprintf("\r\n");
}

void sysStatReset()
{
	memset(&gSysStat, 0, sizeof(struct Stat));
}
