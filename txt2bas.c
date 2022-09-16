#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TOK_COND   0x01
#define TOK_MID    0x02
#define TOK_START  0x04
#define TOK_FNPROC 0x08
#define TOK_LINENO 0x10
#define TOK_REM    0x20
#define TOK_PSEUDO 0x40

struct token {
	char    text[9];
	uint8_t token;
	uint8_t flags;
};

static const struct token tokens[] = {
	{ "AND",      0x80, 0                           },
	{ "ABS",      0x94, 0                           },
	{ "ACS",      0x95, 0                           },
	{ "ADVAL",    0x96, 0                           },
	{ "ASC",      0x97, 0                           },
	{ "ASN",      0x98, 0                           },
	{ "ATN",      0x99, 0                           },
	{ "AUTO",     0xc6, TOK_LINENO                  },
	{ "BGET",     0x9a, TOK_COND                    },
	{ "BPUT",     0xd5, TOK_MID|TOK_COND            },
	{ "COLOUR",   0xfb, TOK_MID                     },
	{ "CALL",     0xd6, TOK_MID                     },
	{ "CHAIN",    0xd7, TOK_MID                     },
	{ "CHR$",     0xbd, 0                           },
	{ "CLEAR",    0xd8, TOK_COND                    },
	{ "CLOSE",    0xd9, TOK_MID|TOK_COND            },
	{ "CLG",      0xda, TOK_COND                    },
	{ "CLS",      0xdb, TOK_COND                    },
	{ "COS",      0x9b, 0                           },
	{ "COUNT",    0x9c, TOK_COND                    },
	{ "DATA",     0xdc, TOK_REM                     },
	{ "DEG",      0x9d, 0                           },
	{ "DEF",      0xdd, 0                           },
	{ "DELETE",   0xc7, TOK_LINENO                  },
	{ "DIV",      0x81, 0                           },
	{ "DIM",      0xde, TOK_MID                     },
	{ "DRAW",     0xdf, TOK_MID                     },
	{ "ENDPROC",  0xe1, TOK_COND                    },
	{ "END",      0xe0, TOK_COND                    },
	{ "ENVELOPE", 0xe2, TOK_MID                     },
	{ "ELSE",     0x8b, TOK_LINENO|TOK_START        },
	{ "EVAL",     0xa0, 0                           },
	{ "ERL",      0x9e, TOK_COND                    },
	{ "ERROR",    0x85, TOK_START                   },
	{ "EOF",      0xc5, TOK_COND                    },
	{ "EOR",      0x82, 0                           },
	{ "ERR",      0x9f, TOK_COND                    },
	{ "EXP",      0xa1, 0                           },
	{ "EXT",      0xa2, TOK_COND                    },
	{ "FOR",      0xe3, TOK_MID                     },
	{ "FALSE",    0xa3, TOK_COND                    },
	{ "FN",       0xa4, TOK_FNPROC                  },
	{ "GOTO",     0xe5, TOK_LINENO|TOK_MID          },
	{ "GET$",     0xbe, 0                           },
	{ "GET",      0xa5, 0                           },
	{ "GOSUB",	  0xe4, TOK_LINENO|TOK_MID          },
	{ "GCOL",     0xe6, TOK_MID                     },
	{ "HIMEM",    0x93, TOK_PSEUDO|TOK_MID|TOK_COND },
	{ "INPUT",	  0xe8, TOK_MID                     },
	{ "IF",       0xe7, TOK_MID                     },
	{ "INKEY$",   0xbf, 0                           },
	{ "INKEY",    0xa6, 0                           },
	{ "INT",      0xa8, 0                           },
	{ "INSTR(",   0xa7, 0                           },
	{ "LIST",     0xc9, TOK_LINENO                  },
	{ "LINE",     0x86, 0                           },
	{ "LOAD",     0xc8, TOK_MID                     },
	{ "LOMEM",    0x92, TOK_PSEUDO|TOK_MID|TOK_COND },
	{ "LOCAL",    0xea, TOK_MID                     },
	{ "LEFT$(",   0xc0, 0                           },
	{ "LEN",      0xa9, 0                           },
	{ "LET",      0xe9, TOK_START                   },
	{ "LOG",      0xab, 0                           },
	{ "LN",       0xaa, 0                           },
	{ "MID$(",    0xc1, 0                           },
	{ "MODE",     0xeb, TOK_MID                     },
	{ "MOD",      0x83, 0                           },
	{ "MOVE",     0xec, TOK_MID                     },
	{ "NEXT",     0xed, TOK_MID                     },
	{ "NEW",      0xca, TOK_COND                    },
	{ "NOT",      0xac, 0                           },
	{ "OLD",      0xcb, TOK_COND                    },
	{ "ON",       0xee, TOK_MID                     },
	{ "OFF",      0x87, 0                           },
	{ "OR",       0x84, 0                           },
	{ "OPENIN",   0x8e, 0                           },
	{ "OPENOUT",  0xae, 0                           },
	{ "OPENUP",   0xad, 0                           },
	{ "OSCLI",    0xff, TOK_MID                     },
	{ "PRINT",    0xf1, TOK_MID                     },
	{ "PAGE",     0x90, TOK_PSEUDO|TOK_MID|TOK_COND },
	{ "PTR",      0x8f, TOK_PSEUDO|TOK_MID|TOK_COND },
	{ "PI",       0xaf, TOK_COND                    },
	{ "PLOT",     0xf0, TOK_MID                     },
	{ "POINT(",   0xb0, 0                           },
	{ "PROC",     0xf2, TOK_FNPROC|TOK_MID          },
	{ "POS",      0xb1, TOK_COND                    },
	{ "RETURN",   0xf8, TOK_COND                    },
	{ "REPEAT",   0xf5, 0                           },
	{ "REPORT",   0xf6, TOK_COND                    },
	{ "READ",     0xf3, TOK_MID                     },
	{ "REM",      0xf4, TOK_REM                     },
	{ "RUN",      0xf9, TOK_COND                    },
	{ "RAD",      0xb2, 0                           },
	{ "RESTORE",  0xf7, TOK_LINENO|TOK_MID          },
	{ "RIGHT$(",  0xc2, 0                           },
	{ "RND",      0xb3, TOK_COND                    },
	{ "RENUMBER", 0xcc, TOK_LINENO                  },
	{ "STEP",     0x88, 0                           },
	{ "SAVE",     0xcd, TOK_MID                     },
	{ "SGN",      0xb4, 0                           },
	{ "SIN",      0xb5, 0                           },
	{ "SQR",      0xb6, 0                           },
	{ "SPC",      0x89, 0                           },
	{ "STR$",     0xc3, 0                           },
	{ "STRING$(", 0xc4, 0                           },
	{ "SOUND",    0xd4, TOK_MID                     },
	{ "STOP",     0xfa, TOK_COND                    },
	{ "TAN",      0xb7, 0                           },
	{ "THEN",     0x8c, TOK_LINENO|TOK_START        },
	{ "TO",       0xb8, 0                           },
	{ "TAB(",     0x8a, 0                           },
	{ "TRACE",    0xfc, TOK_LINENO|TOK_MID          },
	{ "TIME",     0x91, TOK_PSEUDO|TOK_MID|TOK_COND },
	{ "TRUE",     0xb9, TOK_COND                    },
	{ "UNTIL",    0xfd, TOK_MID                     },
	{ "USR",      0xba, 0                           },
	{ "VDU",      0xef, TOK_MID                     },
	{ "VAL",      0xbb, 0                           },
	{ "VPOS",     0xbc, TOK_COND                    },
	{ "WIDTH",    0xfe, TOK_MID                     }
};

static inline bool is_space(int ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\r');
}

static inline bool is_upper(int ch)
{
	return (ch >= 'A' && ch <= 'Z');
}

static inline bool is_lower(int ch)
{
	return (ch >= 'a' && ch <= 'z');
}

static inline bool is_alpha(int ch)
{
	return is_upper(ch) || is_lower(ch);
}

static inline bool is_digit(int ch)
{
	return (ch >= '0' && ch <= '9');
}

static inline bool is_xdigit(int ch)
{
	return is_digit(ch) || (ch >= 'A' && ch <= 'F');
}

static inline bool is_alnum(int ch)
{
	return is_digit(ch) || is_alpha(ch);
}

static int txt2bas(const char *fn, FILE *fp)
{
	char txtline[256], basline[256];
	basline[0] = 0x0d;
	while (fgets(txtline, sizeof(txtline), fp)) {
		const char *txtptr = txtline;
		char *basptr = basline + 4;
		int ch = *txtptr++;
		while (is_space(ch))
			ch = *txtptr++;
		if (is_digit(ch)) {
			unsigned lineno = 0;
			do {
				lineno = lineno * 10 + ch - '0';
				ch = *txtptr++;
			} while (is_digit(ch));
			basline[1] = (lineno >> 8);
			basline[2] = lineno;
			bool start = true;
			bool toklno = false;
			while (ch && ch != '\n') {
				if (is_space(ch))
					ch = *txtptr++;
				else if (ch == '&') {
					do {
						*basptr++ = ch;
						ch = *txtptr++;
					} while (is_xdigit(ch));
				}
				else if (ch == '"') {
					do {
						*basptr++ = ch;
						ch = *txtptr++;
					} while (ch && ch != '"');
					*basptr++ = ch;
					ch = *txtptr++;
				}
				else if (ch == ':') {
					*basptr++ = ch;
					ch = *txtptr++;
					start = true;
					toklno = true;
				}
				else if (ch == ',') {
					*basptr++ = ch;
					ch = *txtptr++;
				}
				else if (ch == '*') {
					if (start) {
						do {
							*basptr++ = ch;
							ch = *txtptr++;
						} while (ch && ch != '\r' && ch != '\n');
					}
					else {
						*basptr++ = ch;
						ch = *txtptr++;
					}
				}
				else if (is_digit(ch)) {
					if (toklno) {
						lineno = 0;
						do {
							lineno = lineno * 10 + ch - '0';
							ch = *txtptr++;
						} while (is_digit(ch));
						*basptr++ = 0x8d;
						*basptr++ = (((lineno & 0xc0) ^ 0x40) >> 2) | (((lineno & 0xc000) ^ 0x4000) >> 12) | 0x40;
						*basptr++ = (lineno & 0x3f) | 0x40;
						*basptr++ = ((lineno >> 8) & 0x3f) | 0x40;
						toklno = false;
						start = false;
					}
					else {
						do {
							*basptr++ = ch;
							ch = *txtptr++;
						} while (is_digit(ch) || ch == '.' || ch == 'E' || ch == 'e');
					}
				}
				else if (ch == '.') {
					do {
						*basptr++ = ch;
						ch = *txtptr++;
					} while (is_digit(ch) || ch == 'E' || ch == 'e');
				}
				else if (is_lower(ch) || (ch >= 'X' && ch <= 'Z')) {
					do {
						*basptr++ = ch;
						ch = *txtptr++;
					} while (is_upper(ch) || is_lower(ch) || ch == '%' || ch == '$');
				}
				else if (ch >= 'A' && ch <= 'W') {
					const struct token *ptr = tokens;
					const struct token *end = tokens + sizeof(tokens)/sizeof(struct token);
					bool found = false;
					while (ptr < end) {
						int tok_ch = ptr->text[0];
						if (ch < tok_ch)
							break;
						int ix = 0;
						int txt_ch = ch;
						while (txt_ch == tok_ch) {
							txt_ch = txtptr[ix++];
							tok_ch = ptr->text[ix];
						}
						if (txt_ch == '.') {
							txtptr += ix;
							found = true;
							break;
						}
						if (!tok_ch && (!(ptr->flags & TOK_COND) || !is_alnum(txt_ch))) {
							txtptr += ix - 1;
							found = true;
							break;
						}
						ptr++;
					}
					if (found) {
						unsigned token = ptr->token;
						unsigned flags = ptr->flags;
						if (flags & TOK_PSEUDO)
							token += 0x40;
						*basptr++ = token;
						if (flags & TOK_MID) {
							start = false;
							toklno = false;
						}
						if (flags & TOK_START) {
							start = true;
							toklno = false;
						}
						ch = *txtptr++;
						if (flags & TOK_FNPROC) {
							while (is_alpha(ch)) {
								*basptr++ = ch;
								ch = *txtptr++;
							}
						}
						if (flags & TOK_LINENO)
							toklno = true;
						if (flags & TOK_REM) {
							do {
								*basptr++ = ch;
								ch = *txtptr++;
							} while (ch && ch != '\r' && ch != '\n');
						}
					}
					else {
						do {
							*basptr++ = ch;
							ch = *txtptr++;
						} while (is_upper(ch) || is_lower(ch) || ch == '%' || ch == '$');
					}
				}
				else {
					*basptr++ = ch;
					ch = *txtptr++;
				}
			}
			size_t len = basptr - basline;
			basline[3] = len;
			fwrite(basline, len, 1, stdout);
			fflush(stdout);
		}
		else {
			fputs("missing line number\n", stderr);
			return 1;
		}
	}
	basline[1] = 0xff;
	fwrite(basline, 2, 1, stdout);
	return 0;
}

int main(int argc, char **argv)
{
	int status = 0;
	if (--argc) {
		do {
			const char *fn = *++argv;
			FILE *fp = fopen(fn, "r");
			if (fp) {
				if (txt2bas(fn, fp))
					status = 1;
				fclose(fp);
			}
			else {
				fprintf(stderr, "txt2bas: unable to open '%s': %s", fn, strerror(errno));
				status = 1;
			}
		} while (--argc);
	}
	else
		status = txt2bas("stdin", stdin);
	return status;
}
