//
// stm32f1_usb_bdt_layout.h
//
// Created on: June 15, 2024
//     Author: Dmitry Murashov
//

#ifndef SRC_TARGET_ARM_STM32F1_STM32F1_USB_BDT_LAYOUT_H_
#define SRC_TARGET_ARM_STM32F1_STM32F1_USB_BDT_LAYOUT_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include <stdint.h>

/****************************************************************************
* Pre-processor Definitions
****************************************************************************/

#define US_STM32F1_BDT_TABLE_AHB_ADDRESS (0x40006000)

/**
 * @def Define RX = 0, and TX = N, to make 2 buffers to point at the same place
 */
#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 0
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 0 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 1
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 1 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 2
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 2 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 3
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 3 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 4
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 4 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 5
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 5 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 6
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 6 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 7
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE
#    error USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE definition required
#  endif /* USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE */
#  ifndef USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE_TX
#    define USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE_TX (USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE)
#  endif
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 7 */

/****************************************************************************
* Public Types
****************************************************************************/

#ifndef __ASSEMBLY__

typedef struct {
	/// \brief Buffer description table. First 32 16-bit-words (64 bytes overall)
	struct {
		volatile uint32_t addrTx;
		volatile uint32_t countTx;
		volatile uint32_t addrRx;
		volatile uint32_t countRx;
	} bdt[8];

	/* `/ 2`, because the API operates w/ bytes, but BDT uses 16-bit words */
#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 0
    volatile uint32_t ep0RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE / 2];
    volatile uint32_t ep0TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP0_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 0 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 1
    volatile uint32_t ep1RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE / 2];
    volatile uint32_t ep1TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP1_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 1 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 2
    volatile uint32_t ep2RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE / 2];
    volatile uint32_t ep2TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP2_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 2 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 3
    volatile uint32_t ep3RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE / 2];
    volatile uint32_t ep3TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP3_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 3 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 4
    volatile uint32_t ep4RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE / 2];
    volatile uint32_t ep4TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP4_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 4 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 5
    volatile uint32_t ep5RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE / 2];
    volatile uint32_t ep5TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP5_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 5 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 6
    volatile uint32_t ep6RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE / 2];
    volatile uint32_t ep6TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP6_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 6 */

#if USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 7
    volatile uint32_t ep7RxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE / 2];
    volatile uint32_t ep7TxBuffer[USBAD_STM32F1_USB_BDT_LAYOUT_EP7_BUFFER_SIZE_TX / 2];
#endif /* USBAD_STM32F1_USB_BDT_LAYOUT_NENDPOINTS > 7 */

//USBAD_STM32F1_USB_BDT_LAYOUT
} BdtLayout;

/****************************************************************************
* Public Data
****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

EXTERN volatile BdtLayout *gUsbBdt;

/****************************************************************************
* Public Function Prototypes
****************************************************************************/

/****************************************************************************
* Mandatory static data
****************************************************************************/

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif  // SRC_TARGET_ARM_STM32F1_STM32F1_USB_BDT_LAYOUT_H_
