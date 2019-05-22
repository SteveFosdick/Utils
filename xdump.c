#include <errno.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 16
#define OUT_SIZE (12+4*BLOCK_SIZE)

static const char xdigs[] = "0123456789ABCDEF";

static void cvt_offset(long offset, char *out)
{
    char *outp = out + 7;
    while (outp >= out) {
        *outp-- = xdigs[offset & 0x0f];
        offset >>= 4;
    }
}

static void cvt_data(unsigned char *raw, int bytes, char *out)
{
    char *out1, *out2;

    out1 = out + 11;
    out2 = out1 + (3*BLOCK_SIZE);
    while (bytes--) {
        int ch = *raw++;
        *out1++ = xdigs[(ch & 0xf0) >> 4];
        *out1++ = xdigs[ch & 0x0f];
        out1++;
        if (ch < 0x20 || ch > 0x7e)
            ch = '.';
        *out2++ = ch;
    }
}

static void filter(FILE *ifp, FILE *ofp)
{
    unsigned char raw[BLOCK_SIZE];
    char          out[OUT_SIZE];
    char          *outp;
    size_t        nbytes;
    long          offset;

    // Put all the fixed features into the output line.

    outp = out + 8;
    *outp++ = ' ';
    *outp++ = '-';
    for (int i = 0; i < BLOCK_SIZE; i++) {
        *outp = ' ';
        outp += 3;
    }
    *outp = ' ';
    outp += BLOCK_SIZE+1;
    *outp = '\n';

    // Whole blocks.

    offset = 0;
    while ((nbytes = fread(raw, 1, BLOCK_SIZE, ifp)) == BLOCK_SIZE)
    {
        cvt_offset(offset, out);
        cvt_data(raw, BLOCK_SIZE, out);
        fwrite(out, OUT_SIZE, 1, ofp);
        offset += BLOCK_SIZE;
    }

    // Partial end block.

    if (nbytes > 0)
    {
        cvt_offset(offset, out);
        cvt_data(raw, nbytes, out);
        outp = out + 11 + (nbytes * 3);
        for (int i = nbytes; i < BLOCK_SIZE; i++) {
            *outp++ = '*';
            *outp++ = '*';
            outp++;
        }
        outp += nbytes;
        *outp++ = '\n';
        fwrite(out, outp-out, 1, ofp);
    }
}

#include "filter.c"
