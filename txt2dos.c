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

static void newlines(int count, FILE *ofp)
{
    while (count--) {
        putc('\r', ofp);
        putc('\n', ofp);
    }
}

static inline void endout(FILE *ofp) {}

#include "txtconv.c"

/* End */

