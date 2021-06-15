# Utils
This is a set of utilities I wrote because find them useful, primarily
for working with BBC micros and BBC micro emulators, but also for other
things too.
## Text Conversion
The programs txt2bbc, txt2cpm, txt2dos and txt2ux convert text files
so they use the end-of-line convention of the destination system so:
* txt2bbc uses CR line endings (also used by the classic Macintosh),
* txt2cpm uses CR/LF pair and includes the final control Z needed
  because CP/M does not keep an exact file length, only the number of
  128 byte records.
* txt2dos also uses CR/LF line endings but omits the final ^Z
* txt2ux uses LF line endings for Unix family OSes including the
  branded Unixes, GNU/Linux, the BSDs and MacOS.
Internally, all these utils use txtconv.c which contains a simple finite
state machine to enable it to be able to handle any of the end-of-line
conventions on input.
## Hex Dump
The Disk Filing System (DFS) on the BBC micro included a command *DUMP
which produced a nice listing with the file offset, the bytes displayed
in hex, then the same bytes again in ASCII (where the byte is an ASCII
character otherwise a dot).  The xdump program uses the same format but
with 16 bytes per line instead of 8 as this fits nicely into an 80
column display.
## Double-Sided Floppy Disc Conversion
Header-less sectors dumps of floppy discs are common working with BBC
micro emulators.  The most common, SSD, is simple enough having all the
sectors of track 0 in order, followed by track 1 etc.  Most
double-sided images, i.e. DSD files, have the tracks from the two sides
interleaved, i.e. side 0 track 0, side 1, track 0, side 0 track 1, etc.

The programs fdsplit and fdcombine allow conversion between these DSD
files and a pairs of SSD files for the two sides thus doing the
interleaving or de-interleaving.  By default 80 tracks of 10 sectors
each is assumed (Acorn single density DFS format) but each program
takes -t and -s options to override this if working with one of the
third party double density DFSes.
## Recover Clean ASCII Text
The clean7 command extracts from a file only the characters that are
printable ASCII, tab or newline.
