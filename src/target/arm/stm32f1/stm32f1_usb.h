//
// stm32_usb.h
//
// Created on: March 28, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//
// A bunch of inline boilerplate reducers
//

#ifndef SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_
#define SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_

/****************************************************************************
* Included Files
****************************************************************************/

#include <stm32f103x6.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************
* Pre-processor Definitions
****************************************************************************/

/// \var Certain bits are toggled when written 1, others are just regular rw, or rc_w0
#define USBAD_USB_EPXR_NON_TOGGLE_BITS (USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_EA)

/****************************************************************************
* Public Types
****************************************************************************/

#ifndef __ASSEMBLY__

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

/****************************************************************************
* Public Function Prototypes
****************************************************************************/

/// \brief API for interfacing w/ STM's USB BDT (RM0008 Rev 21 p 648)
/// \pre aOutBuffer is supposed to be able to accomodate `aReadSequenceLength`
/// half-words
/// \arg `aUsbBdtInnerOffset` -- inner offset in the buffer table descriptor
/// memory
/// \pre No fool protectione is used. The size of BDT must be accounted for
void usStm32f1UsbReadBdt(uint16_t *aOutBuffer, size_t aReadSequenceLength, size_t aUsbBdtInnerOffset);

/// \brief Writes `aInBuffer` into USB BDT memory
/// \pre No fool protectione is used. The size of BDT must be accounted for
/// \details Quite similar to `usStm32UsbReadBdt`. Refer to its description
void usStm32f1UsbWriteBdt(uint16_t *aInBuffer, size_t aWriteSequenceLength, size_t aUsbBdtInnerOffset);

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

/// \param `aWriteSequenceLength` -- in half-words
static inline void usStm32f1UsbSetBdt(uint16_t aValue, size_t aWriteSequenceLength, size_t aUsbBdtInnerOffset)
{
	while (aWriteSequenceLength) {
		usStm32f1UsbWriteBdt(&aValue, 1, aUsbBdtInnerOffset);
		aUsbBdtInnerOffset += 2;
		aWriteSequenceLength -= 2;
	}
}

/// \pre BTABLE register must be set
static inline void setUsbAddrnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void setUsbAddrnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 4;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void setUsbCountnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 2;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void setUsbCountnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 6;
	usStm32f1UsbWriteBdt(&aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbAddrnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbCountnTx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 2;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbAddrnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 4;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \pre BTABLE register must be set
/// \warning Calculated w/ account that no double buffering is being used for
/// any particular enpoint. Otherwise, not valid, and should not be used
static inline void getUsbCountnRx(volatile USB_TypeDef *aUsb, uint8_t aEp, uint16_t *aValue)
{
	size_t bdtInnerOffset = aUsb->BTABLE + aEp * 8 + 6;
	usStm32f1UsbReadBdt(aValue, 1, bdtInnerOffset);
}

/// \brief Get minimum required BDT offset based on the number of endpoints
/// The more the number of endpoints is, the bigger the table must be
/// RM0008 rev 21 p 650
/// \param aEndpoint total number of EPs
static inline size_t getMinInnerBdtOffset(size_t aEndpoints)
{
	return 64;
}

static inline volatile uint16_t *getEpxr(uint8_t aEndpointNumber)
{
	return &USB->EP0R + 2 * aEndpointNumber;
}

/// \brief Automatically decides whether a bit is of "w-1-toggle" type, and applies applicable setting strategy
/// \returns previous value
/// \post CTR_RX and CTR_TX will be cleared as a result of reading from the register
static uint16_t setEpxrBits(volatile USB_TypeDef *aUsb, uint8_t aEndpoint, uint16_t aValue, uint16_t aOffset,
	uint16_t aMask)
{
	volatile uint16_t *epxr = getEpxr(aEndpoint);
	const uint16_t previousValue = *epxr;

	if (aMask & USBAD_USB_EPXR_NON_TOGGLE_BITS) {
		// TODO: previous value is not cleared
		*epxr = (previousValue & USBAD_USB_EPXR_NON_TOGGLE_BITS) | (aValue << aOffset);
	} else {
		*epxr = (previousValue & USBAD_USB_EPXR_NON_TOGGLE_BITS) | ((previousValue ^ (aValue << aOffset)) & aMask);
	}

	return previousValue;
}

/// \returns Previous value
static inline uint16_t setEpxrEpType(uint8_t aEndpoint, uint8_t aType)
{
	volatile uint16_t *epxr = getEpxr(aEndpoint);
	uint16_t ret = *epxr;

	*epxr = (ret & (USB_EP0R_EA | USB_EP0R_EP_KIND)) | aType << USB_EP0R_EP_TYPE_Pos;

	return ret;
}

/// \brief EPxR has both set-toggle and set-write mode bits. This one handles the former.
static inline uint16_t setEpxrToggle(uint8_t aEndpoint, uint16_t aValue, uint32_t aOffset, uint32_t aMask)
{
	volatile uint16_t *epxr = getEpxr(aEndpoint);
	uint16_t ret = *epxr;

	*epxr = (ret & USBAD_USB_EPXR_NON_TOGGLE_BITS) | ((ret ^ (aValue << aOffset)) & aMask);

	return ret;
}

/// \brief EPxR has both set-toggle and set-write mode bits. This one handles the latter.
static inline uint16_t setEpxrNonToggle(uint8_t aEndpoint, uint32_t aValue, uint32_t aOffset, uint32_t aMask)
{
	volatile uint16_t epxr = *getEpxr(aEndpoint);
	*getEpxr(aEndpoint) = (epxr & USBAD_USB_EPXR_NON_TOGGLE_BITS & ~aMask) | (aValue << aOffset);

	return epxr;
}

static inline uint16_t setEpxrStatTx(uint8_t aEndpoint, uint32_t aValue)
{
	return setEpxrToggle(aEndpoint, aValue, USB_EP0R_STAT_TX_Pos, USB_EP0R_STAT_TX_Msk);
}

static inline uint16_t setEpxrStatRx(uint8_t aEndpoint, uint32_t aValue)
{
	return setEpxrToggle(aEndpoint, aValue, USB_EP0R_STAT_RX_Pos, USB_EP0R_STAT_RX_Msk);
}

static inline uint16_t setEpxrDtogTx(uint8_t aEndpoint, uint32_t aValue)
{
	return setEpxrToggle(aEndpoint, aValue, USB_EP0R_DTOG_TX_Pos, USB_EP0R_DTOG_TX_Msk);
}

static inline uint16_t setEpxrDtogRx(uint8_t aEndpoint, uint32_t aValue)
{
	return setEpxrToggle(aEndpoint, aValue, USB_EP0R_DTOG_RX_Pos, USB_EP0R_DTOG_RX_Msk);
}

static inline uint16_t resetEpxrCtrRx(uint16_t aEndpoint)
{
	return setEpxrNonToggle(aEndpoint, 0, USB_EP0R_CTR_RX_Pos, USB_EP0R_CTR_RX_Msk);
}

static inline uint16_t resetEpxrCtrTx(uint16_t aEndpoint)
{
	return setEpxrNonToggle(aEndpoint, 0, USB_EP0R_CTR_TX_Pos, USB_EP0R_CTR_TX_Msk);
}

static inline uint16_t getEpxAddrnRxOffset(uint8_t aMaxEndpoints, uint16_t aBufferSize, uint8_t aEpx)
{
	return getMinInnerBdtOffset(aMaxEndpoints) + aBufferSize * aEpx * 2;
}

static inline uint16_t getEpxAddrnTxOffset(uint8_t aMaxEndpoints, uint16_t aBufferSize, uint8_t aEpx)
{
	return getEpxAddrnRxOffset(aMaxEndpoints, aBufferSize, aEpx) + aBufferSize;
}

#undef EXTERN
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ASSEMBLY__ */

#endif  // SRC_COMMON_TARGET_ARM_STM32_STM32_USB_H_
