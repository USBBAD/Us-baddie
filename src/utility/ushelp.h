//
// ushelp.h
//
// Created on: Dec 10, 2023
//     Author: Dmitry Murashov (dmtr <DOT> murashov <AT> <GMAIL> <DOT> <COM>)
//

/// \brief Various helpers

#ifndef USBAD_USHELP_H_
#define USBAD_USHELP_H_

#define US_MIN(a, b) ((a) > (b) ? (b) : (a))
#define US_MAX(a, b) ((a) < (b) ? (b) : (a))
#define US_GET_BIT(value, mask, offset) ((value & mask) >> offset)
#define US_CLAMP(a, b, val) (US_MAX(a, US_MIN(b, val)))
#define US_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#endif // USBAD_USHELP_H_
