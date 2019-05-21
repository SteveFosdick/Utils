/*
 * txtconv.c
 *
 * This is the core of a text line-ending and file-ending conversion
 * filter which just needs the addition of functions to write the
 * correct end-of-line and end-of-file sequences for the destination
 * platform.
 *
 * As a "filter" function it expects to be driven from a standard
 * main program that calls it for each input file or for stdin and
 * writes the output to stdout.
 */

#include <stdio.h>

typedef enum {
    ST_GROUND,
    ST_GOT_CR,
    ST_GOT_LF,
    ST_GOT_BOTH
} state_t;

void filter(FILE *fp) {
    state_t state = ST_GROUND;
    int ch, lines = 0, spaces = 0;

    while ((ch = getc(fp)) != EOF) {
        if (ch == '\r') {
            if (state == ST_GOT_LF)
                state = ST_GOT_BOTH;
            else {
                lines++;
                state = ST_GOT_CR;
            }
       }
       else if (ch == '\n') {
           if (state == ST_GOT_CR)
               state = ST_GOT_BOTH;
            else {
                lines++;
                state = ST_GOT_LF;
            }
        }
        else {
            if (state != ST_GROUND) {
                newlines(lines);
                lines = spaces = 0;
                state = ST_GROUND;
            }
            if (ch == ' ')
                spaces++;
            else {
                while (spaces > 0) {
                    putchar(' ');
                    spaces--;
                }
                putchar(ch);
            }
        }
    }
    if (state != ST_GROUND && lines > 0)
        newlines(lines);
}
