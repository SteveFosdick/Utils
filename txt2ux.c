/*
 * txt2ux.c
 *
 * This file provides the correct newline and end of file output
 * functions for Unix format text files.
 *
 * Combined with txtconv.c and filter.c it produces a complete program
 * for translating the line endings in a text files from other OS
 * formats to Unix format.
 */

#include <stdio.h>

static void newlines(int count, FILE *ofp)
{
    while (count--) {
        putc('\n', ofp);
    }
}

static inline void endout(FILE *ofp) {}

#include "txtconv.c"

/* End */

