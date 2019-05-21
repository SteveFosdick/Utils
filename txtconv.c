/*
 * txtconv.c
 *
 * This is the core of a text line-ending and file-ending conversion
 * filter which just needs the addition of functions to write the
 * correct end-of-line and end-of-file sequences for the destination
 * platform.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ST_GROUND,
    ST_GOT_CR,
    ST_GOT_LF,
    ST_GOT_BOTH
} state_t;

static void txtconv(FILE *ifp, FILE *ofp) {
    state_t state = ST_GROUND;
    int ch, lines = 0, spaces = 0;

    while ((ch = getc(ifp)) != EOF) {
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
                newlines(lines, ofp);
                lines = spaces = 0;
                state = ST_GROUND;
            }
            if (ch == ' ')
                spaces++;
            else {
                while (spaces > 0) {
                    putc(' ', ofp);
                    spaces--;
                }
                putc(ch, ofp);
            }
        }
    }
    newlines(lines, ofp);
}

static const char usage[]  = "Usage: %s [ -b -r ] [ <file> ... ]\n";
static const char memerr[] = "%s: out of memory\n";
static const char renerr[] = "%s: unable to rename %s to %s: %m\n";
static const char orderr[] = "%s: unable to open %s for reading: %m\n";
static const char owterr[] = "%s: unable to open %s for writing: %m\n";

static int replace(const char *progname, int argc, char **argv, bool back)
{
    int status = 0;

    if (argc == 0) {
        fprintf(stderr, usage, progname);
        status = 1;
    }
    else {
        while (argc--) {
            const char *fn = *argv++;
            FILE *ifp = fopen(fn, "rb");
            if (ifp) {
                size_t len = strlen(fn);
                char *new = malloc(len+2);
                if (new) {
                    memcpy(new, fn, len);
                    new[len] = '$';
                    new[len+1] = 0;
                    FILE *ofp = fopen(new, "wb");
                    if (ofp) {
                        txtconv(ifp, ofp);
                        endout(ofp);
                        fclose(ofp);
                        if (back) {
                            char *bak = malloc(len+2);
                            if (bak) {
                                memcpy(bak, fn, len);
                                bak[len] = '~';
                                bak[len+1] = 0;
                                if (rename(fn, bak) == 0) {
                                    if (rename(new, fn) != 0) {
                                        fprintf(stderr, renerr, progname, new, fn);
                                        status = 1;
                                    }
                                }
                                else {
                                    fprintf(stderr, renerr, progname, fn, bak);
                                    status = 1;
                                }
                                free(bak);
                            }
                            else {
                                fprintf(stderr, memerr, progname);
                                status = 1;
                            }
                        }
                        else if (rename(new, fn) != 0) {
                            fprintf(stderr, renerr, progname, new, fn);
                            status = 1;
                        }
                    }
                    else {
                        fprintf(stderr, owterr, progname, new);
                        status = 1;
                    }
                    free(new);
                }
                else {
                    fprintf(stderr, memerr, progname);
                    status = 1;
                }
                fclose(ifp);
            }
            else {
                fprintf(stderr, orderr, progname, fn);
                status = 1;
            }
        }
    }
    return status;
 }

static int tostdout(const char *progname, int argc, char **argv)
{
    int status = 0;

    if (argc == 0)
        txtconv(stdin, stdout);
    else {
        while (--argc) {
            const char *fn = *argv++;
            if (fn[0] == '-' && fn[1] == '\0')
                txtconv(stdin, stdout);
            else {
                FILE *ifp = fopen(fn, "rb");
                if (ifp) {
                    txtconv(ifp, stdout);
                    fclose(ifp);
                }
                else {
                    fprintf(stderr, orderr, progname, fn);
                    status = 1;
                }
            }
        }
    }
    endout(stdout);
    return status;
 }

int main(int argc, char **argv) {
    int status = 0;
    const char *progname;
    bool back = false;
    bool repl = false;

    progname = *argv;
    while (--argc) {
        const char *arg = *++argv;
        if (*arg != '-')
            break;
        int ch;
        while ((ch = *++arg)) {
            if (ch == 'b')
                back = 1;
            else if (ch == 'r')
                repl = true;
            else {
                status = 1;
                break;
            }
        }
    }
    if (status == 0) {
        if (repl)
            status = replace(progname, argc, argv, back);
        else
            status = tostdout(progname, argc, argv);
    }
    else {
        fprintf(stderr, usage, progname);
        status = 1;
    }
    return status;
}
