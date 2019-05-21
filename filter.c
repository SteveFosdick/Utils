/*
 * filter.c
 *
 * This is a main program for any one of a number of simple utilities
 * that each work as a filter, i.e. a programs that can be in a pipe,
 * or take one or more files as parameters and output to stdout.
 */

#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int status = 0;
    const char *progname, *filename, *ptr;
    FILE *fp;

    if (argc == 1)
        filter(stdin);
    else {
        progname = *++argv;
        if ((ptr = strrchr(progname, '.')))
            progname = ptr+1;
        while (--argc) {
            filename = *argv++;
        if (filename[0] == '-' && filename[1] == '\0')
        filter(stdin);
            else if ((fp = fopen(filename, "rb"))) {
                filter(fp);
                fclose(fp);
            } else {
                fprintf(stderr, "%s: unable to open %s for reading: %m\n",
                        progname, filename);
                status++;
            }
        }
    }
    endout();
    return status;
}
