/*
 * txt2dos.c
 *
 * This file provides the correct newline and end of file output
 * functions for DOS/Windows.
 *
 * Combined with txtconv.c and filter.c it produces a complete program
 * for translating the line endings in a text files from other OS
 * formats to DOS/Windows.
 */

#include <stdio.h>

static void newlines(int count) {
    while (count--) {
        putchar('\r');
        putchar('\n');
    }
}

static inline void endout(void) {}

#include "txtconv.c"
#include "filter.c"

/* End */

