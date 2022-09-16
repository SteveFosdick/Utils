#include <errno.h>
#include <stdio.h>
#include <string.h>

#define GROUP_SIZE  8
#define BLOCK_SIZE (GROUP_SIZE * 2)
#define OUT_SIZE (14+4*BLOCK_SIZE)

static const char xdigs[] = "0123456789ABCDEF";

static char *hex_group(unsigned char *raw, int bytes, char *out)
{
    while (bytes--) {
        int ch = *raw++;
        *out++ = xdigs[(ch & 0xf0) >> 4];
        *out++ = xdigs[ch & 0x0f];
        out++;
    }
    return out;
}

static void rest_out(long offset, unsigned char *raw, size_t bytes, char *out, size_t out_size, FILE *ofp)
{
    // Offset into file.

    char *outp = out + 7;
    while (outp >= out) {
        *outp-- = xdigs[offset & 0x0f];
        offset >>= 4;
    }

    // ASCII

    outp = out + 13 + BLOCK_SIZE*3;
    while (bytes--) {
        int ch = *raw++;
        if (ch < 0x20 || ch > 0x7e)
            ch = '.';
        *outp++ = ch;
    }

    // Output

    fwrite(out, out_size, 1, ofp);
}

static char *star_pad(size_t count, char *out)
{
    while (count--) {
        *out++ = '*';
        *out++ = '*';
        out++;
    }
    return out;
}

static void filter(FILE *ifp, FILE *ofp)
{
    unsigned char raw[BLOCK_SIZE];
    char          out[OUT_SIZE];
    size_t        nbytes;

    // Put all the fixed features into the output line.

    memset(out, ' ', OUT_SIZE);
    out[9] = '-';
    out[OUT_SIZE-1] = '\n';

    // Whole blocks.

    long offset = 0;
    while ((nbytes = fread(raw, 1, BLOCK_SIZE, ifp)) == BLOCK_SIZE) {
        char *outp = hex_group(raw, GROUP_SIZE, out + 11);
        hex_group(raw + GROUP_SIZE, GROUP_SIZE, outp+1);
        rest_out(offset, raw, BLOCK_SIZE, out, OUT_SIZE, ofp);
        offset += BLOCK_SIZE;
    }

    // Partial end block.

    if (nbytes > 0) {
        size_t out_size = 14 + 3*BLOCK_SIZE + nbytes;
        out[out_size-1] = '\n';
        if (nbytes >= GROUP_SIZE) {
            char *outp = hex_group(raw, GROUP_SIZE, out + 11);
            outp = hex_group(raw + GROUP_SIZE, nbytes - GROUP_SIZE, outp+1);
            star_pad(BLOCK_SIZE - nbytes, outp);
        }
        else {
            char *outp = hex_group(raw, nbytes, out + 11);
            outp = star_pad(GROUP_SIZE - nbytes, outp);
            star_pad(GROUP_SIZE, outp+1);
        }
        rest_out(offset, raw, nbytes, out, out_size, ofp);
    }
}

#include "filter.c"
