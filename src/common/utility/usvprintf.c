//
// vprintf.c
//
// Created on: March 17, 2024
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#include "cbprintf.h"

puts_t *gVprintfCallback = 0;

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_MIN 64
#define STB_SPRINTF_NOFLOAT
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOUNALIGNED  // Required on PIC32 or printing will cause Adress Exception on load

#include "stb_sprintf.h"

static char *rb_putcb(char *buf, void *user, int len) {
    puts_t *callback = (puts_t *)user;
    size_t  ln       = len;  // explicit cast
    if (callback(buf, len) < ln) {
        return NULL;
    }
    return buf;
}

int usvprintf(const char *fmt, ...) {
    va_list ap;
    int rv;
    char b[STB_SPRINTF_MIN];
    va_start(ap, fmt);

    if (gVprintfCallback) {
		stbsp_set_separators('\'', '.');
		rv = stbsp_vsprintfcb(rb_putcb, gVprintfCallback, b, fmt, ap);
    }

    va_end(ap);
    return rv;
}
