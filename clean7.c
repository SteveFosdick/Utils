#include <stdio.h>

void filter(FILE *in, FILE *out)
{
    int ch;
    while ((ch = getc(in)) != EOF)
        if ((ch >= ' ' && ch <= '~') || ch == '\t' || ch == '\r' || ch == '\n')
            putc(ch, out);
}

#include "filter.c"
