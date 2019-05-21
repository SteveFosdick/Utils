/*
 * txt2dos.c
 *
 * This file provides the correct newline and end of file output
 * functions for CP/M format text files.
 *
 * Combined with txtconv.c and filter.c it produces a complete program
 * for translating the line endings in a text files from other OS
 * formats to CP/M format.
 */

#include <stdio.h>

static void newlines(int count, FILE *ofp)
{
    while (count--) {
        putc('\r', ofp);
        putc('\n', ofp);
    }
}

static inline void endout(FILE *ofp)
{
    putc(0x1a, ofp);
}

#include "txtconv.c"

/* End */

