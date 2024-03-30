//
// cbprintf.h
//
// Created on: ?
//     Author: https://github.com/daedaleanai/stm32f103_usb.git
//

#ifndef SRC_COMMON_UTILITY_CBPRINTF_H_
#define SRC_COMMON_UTILITY_CBPRINTF_H_

/*
    Everything you need to implement a printf to serial or similar devices.
    The implementation is done by stb_printf.h

    Typically you make a puts_t callback to copy to ringbuffer and enable the irq or dma
    that empties it, and in the irq/dma handler you pull from the ringbuffer and switch it off when empty.
*/

#include <stdarg.h>  // for varargs
#include <stddef.h>  // for size_t
#include <stdint.h>  // for uintX_t

// puts_t is the type of a callback called by cbprintf() repeatedly.
// a puts(..)-like function should try to output buf[0:len], and return
// the number of characters actually copied out. if the returned value is less
// than len, cbprintf() will return immediately without attempting to print more.
typedef size_t puts_t(const char *buf, size_t len);

// cbprintf() interprets fmt as a format string for the variable parameters and calls the callback to
// copy the characters out, up to 64 at a time.  When the callback returns less than len, printing
// is aborted.
int cbprintf(puts_t *callback, const char *fmt, ...) __attribute__((format(cbprintf, 2, 3)));

#endif  // SRC_COMMON_UTILITY_CBPRINTF_H_
