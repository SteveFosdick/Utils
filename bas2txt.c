#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct token {
    char text[9];
    uint8_t flags;
};

#define SPC_BEFORE 0x01
#define SPC_AFTER  0x02
#define INC_INDENT 0x04
#define DEC_INDENT 0x08

static const struct token tokens[] = {
    /* Operators */
    { "AND",      SPC_BEFORE|SPC_AFTER }, // 80
    { "DIV",      SPC_BEFORE|SPC_AFTER }, // 81
    { "EOR",      SPC_BEFORE|SPC_AFTER }, // 82
    { "MOD",      SPC_BEFORE|SPC_AFTER }, // 83
    { "OR",       SPC_BEFORE|SPC_AFTER }, // 84
    /* Auxilliary tokens */
    { "ERROR",    SPC_AFTER            }, // 85
    { "LINE",     SPC_AFTER            }, // 86
    { "OFF",      SPC_AFTER            }, // 87
    { "STEP",     SPC_BEFORE|SPC_AFTER }, // 88
    { "SPC",      SPC_AFTER            }, // 89
    { "TAB(",     0                    }, // 8A
    { "ELSE",     SPC_BEFORE|SPC_AFTER }, // 8B
    { "THEN",     SPC_BEFORE|SPC_AFTER }, // 8C
    /* Line number token */
    { "",         0                    }, // 8D
    /* Oddly placed as added with BASIC 2 */
    { "OPENIN",   SPC_AFTER            }, // 8E
    /* Pseudo variable functions */
    { "PTR",      0                    }, // 8F
    { "PAGE",     0                    }, // 90
    { "TIME",     0                    }, // 91
    { "LOMEM",    0                    }, // 92
    { "HIMEM",    0                    }, // 93
    /* Numeric valued functions */
    { "ABS",      0                    }, // 94
    { "ACS",      0                    }, // 95
    { "ADVAL",    0                    }, // 96
    { "ASC",      0                    }, // 97
    { "ASN",      0                    }, // 98
    { "ATN",      0                    }, // 99
    { "BGET",     0                    }, // 9A
    { "COS",      0                    }, // 9B
    { "COUNT",    0                    }, // 9C
    { "DEG",      0                    }, // 9D
    { "ERL",      0                    }, // 9E
    { "ERR",      0                    }, // 9F
    { "EVAL",     0                    }, // A0
    { "EXP",      0                    }, // A1
    { "EXT",      0                    }, // A2
    { "FALSE",    0                    }, // A3
    { "FN",       0                    }, // A4
    { "GET",      0                    }, // A5
    { "INKEY",    0                    }, // A6
    { "INSTR(",   0                    }, // A7
    { "INT",      0                    }, // A8
    { "LEN",      0                    }, // A9
    { "LN",       0                    }, // AA
    { "LOG",      0                    }, // AB
    { "NOT",      0                    }, // AC
    { "OPENUP",   0                    }, // AD
    { "OPENOUT",  0                    }, // AE
    { "PI",       0                    }, // AF
    { "POINT(",   0                    }, // B0
    { "POS",      0                    }, // B1
    { "RAD",      0                    }, // B2
    { "RND",      0                    }, // B3
    { "SGN",      0                    }, // B4
    { "SIN",      0                    }, // B5
    { "SQR",      0                    }, // B6
    { "TAN",      0                    }, // B7
    { "TO",       SPC_BEFORE|SPC_AFTER }, // B8
    { "TRUE",     0                    }, // B9
    { "USR",      0                    }, // BA
    { "VAL",      0                    }, // BB
    { "VPOS",     0                    }, // BC
    { "CHR$",     0                    }, // BD
    /* String-valued functions */
    { "GET$",     0                    }, // BE
    { "INKEY$",   0                    }, // BF
    { "LEFT$(",   0                    }, // C0
    { "MID$(",    0                    }, // C1
    { "RIGHT$(",  0                    }, // C2
    { "STR$",     0                    }, // C3
    { "STRING$(", 0                    }, // C4
    /* EOF is an odd-ball */
    { "EOF",      0                    }, // C5
    /* Commands */
    { "AUTO",     0                    }, // C6
    { "DELETE",   0                    }, // C7
    { "LOAD",     0                    }, // C8
    { "LIST",     0                    }, // C9
    { "NEW",      0                    }, // CA
    { "OLD",      0                    }, // CB
    { "RENUMBER", 0                    }, // CC
    { "SAVE",     0                    }, // CD
    { "",         0                    }, // CE
    /* Pseudo-variable statements */
    { "PTR",      0                    }, // CF
    { "PAGE",     0                    }, // D0
    { "TIME",     0                    }, // D1
    { "LOMEM",    0                    }, // D2
    { "HIMEM",    0                    }, // D3
    /* Statements */
    { "SOUND",    SPC_AFTER            }, // D4
    { "BPUT",     SPC_AFTER            }, // D5
    { "CALL",     SPC_AFTER            }, // D6
    { "CHAIN",    SPC_AFTER            }, // D7
    { "CLEAR",    SPC_AFTER            }, // D8
    { "CLOSE",    SPC_AFTER            }, // D9
    { "CLG",      SPC_AFTER            }, // DA
    { "CLS",      SPC_AFTER            }, // DB
    { "DATA",     SPC_AFTER            }, // DC
    { "DEF",      SPC_AFTER            }, // DD
    { "DIM",      SPC_AFTER            }, // DE
    { "DRAW",     SPC_AFTER            }, // DF
    { "END",      SPC_AFTER            }, // E0
    { "ENDPROC",  SPC_AFTER            }, // E1
    { "ENVELOPE", SPC_AFTER            }, // E2
    { "FOR",      SPC_AFTER|INC_INDENT }, // E3
    { "GOSUB",    SPC_AFTER            }, // E4
    { "GOTO",     SPC_AFTER            }, // E5
    { "GCOL",     SPC_AFTER            }, // E6
    { "IF",       SPC_AFTER            }, // E7
    { "INPUT",    SPC_AFTER            }, // E8
    { "LET",      SPC_AFTER            }, // E9
    { "LOCAL",    SPC_AFTER            }, // EA
    { "MODE",     SPC_AFTER            }, // EB
    { "MOVE",     SPC_AFTER            }, // EC
    { "NEXT",     SPC_AFTER|DEC_INDENT }, // ED
    { "ON",       SPC_AFTER            }, // EE
    { "VDU",      SPC_AFTER            }, // EF
    { "PLOT",     SPC_AFTER            }, // F0
    { "PRINT",    SPC_AFTER            }, // F1
    { "PROC",     0                    }, // F2
    { "READ",     SPC_AFTER            }, // F3
    { "REM",      SPC_AFTER            }, // F4
    { "REPEAT",   SPC_AFTER|INC_INDENT }, // F5
    { "REPORT",   SPC_AFTER            }, // F6
    { "RESTORE",  SPC_AFTER            }, // F7
    { "RETURN",   SPC_AFTER            }, // F8
    { "RUN",      SPC_AFTER            }, // F9
    { "STOP",     SPC_AFTER            }, // FA
    { "COLOUR",   SPC_AFTER            }, // FB
    { "TRACE",    SPC_AFTER            }, // FC
    { "UNTIL",    SPC_AFTER|DEC_INDENT }, // FD
    { "WIDTH",    SPC_AFTER            }, // FE
    { "OSCLI",    SPC_AFTER            }  // FF
};

static int bas2txt(const char *fn, FILE *fp)
{
    int status = 0, indent = 0;
    unsigned char hdr[4];
    size_t nbytes = fread(hdr, 1, sizeof(hdr), fp);
    if (nbytes == 4 && hdr[0] == 0x0d) {
        do {
            unsigned len = hdr[3] - 4;
            unsigned char line[253];
            if (fread(line, len, 1, fp) != 1) {
                status = 1;
                break;
            }
            printf("%5u ", hdr[1] << 8 | hdr[2]);
            /* pre-scan the line for any change in indent. */
            int delta = 0;
            const unsigned char *ptr = line;
            const unsigned char *end = line + len;
            while (ptr < end) {
                int ch = *ptr++;
                if (ch & 0x80) {
                    const struct token *t = tokens + (ch & 0x7f);
                    unsigned flags = t->flags;
                    if (flags & INC_INDENT)
                        delta++;
                    else if (flags & DEC_INDENT)
                        delta--;
                }
            }
            /* process a decreased indent before printing the line */
            if (delta < 0)
                indent += delta;
            for (int i = indent; i; --i) {
                putchar(' ');
                putchar(' ');
            }
            /* now print the line */
            bool did_space = true;
            bool need_space = false;
            ptr = line;
            while (ptr < end) {
                int ch = *ptr++;
                if (ch & 0x80) {
                    if (ch == 0x8d) {
                        unsigned b1 = ptr[0];
                        unsigned lsb = ((b1 & 0x30) << 2) ^ ptr[1];
                        unsigned msb = ((b1 & 0x0c) << 4) ^ ptr[2];
                        printf(" %u", (msb << 8) | lsb);
                        ptr += 3;
                    }
                    else {
                        const struct token *t = tokens + (ch & 0x7f);
                        unsigned flags = t->flags;
                        if (!did_space && (need_space || (flags & SPC_BEFORE)))
                            putchar(' ');
                        fputs(t->text, stdout);
                        did_space = need_space = false;
                        if (flags & SPC_AFTER)
                            need_space = true;
                    }
                }
                else {
                    if (ch == ' ' || ch == ':') {
                        need_space = false;
                        did_space = true;
                    }
                    else if (need_space && !did_space) {
                        need_space = false;
                        putchar(' ');
                    }
                    putchar(ch);
                }
            }
            putchar('\n');
            if (delta > 0)
                indent += delta;
            nbytes = fread(hdr, 1, sizeof(hdr), fp);
        } while (nbytes == 4 && hdr[0] == 0x0d);

        if (status || nbytes != 2 || hdr[0] != 0x0d || hdr[1] != 0xff) {
            fprintf(stderr, "%s: premature EOF/corrupt BASIC program", fn);
            status = 2;
        }
    }
    else if (nbytes == 0)
        fprintf(stderr, "%s: empty file\n", fn);
    else if (nbytes != 2 || hdr[0] != 0x0d || hdr[1] != 0xff) {
        fprintf(stderr, "%s: not a BASIC program", fn);
        status = 2;
    }
    return status;
}

int main(int argc, char **argv)
{
    int status = 0;
    if (--argc) {
        do {
            const char *fn = *++argv;
            FILE *fp = fopen(fn, "rb");
            if (fp) {
                if (bas2txt(fn, fp))
                    status = 1;
                fclose(fp);
            }
            else {
                fprintf(stderr, "bas2txt: unable to open '%s': %s", fn, strerror(errno));
                status = 1;
            }
        } while (--argc);
    }
    else
        status = bas2txt("stdin", stdin);
    return status;
}
