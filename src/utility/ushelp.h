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

#define US_GET_BIT(value, mask, offset) ((value & mask) >> offset)

#endif // USBAD_USHELP_H_
