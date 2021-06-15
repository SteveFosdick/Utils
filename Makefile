CFLAGS = -O2 -Wall
LDFLAGS = -O2
PROGS  = clean7 txt2bbc txt2dos txt2ux txt2cpm xdump fdcombine fdsplit

all: $(PROGS)

clean7: clean7.c filter.c
	$(CC) $(CFLAGS) -o clean7 clean7.c

txt2bbc: txt2bbc.c txtconv.c
	$(CC) $(CFLAGS) -o txt2bbc txt2bbc.c

txt2dos: txt2dos.c txtconv.c
	$(CC) $(CFLAGS) -o txt2dos txt2dos.c

txt2cpm: txt2cpm.c txtconv.c
	$(CC) $(CFLAGS) -o txt2cpm txt2cpm.c

txt2ux: txt2ux.c txtconv.c
	$(CC) $(CFLAGS) -o txt2ux txt2ux.c

xdump: xdump.c filter.c
	$(CC) $(CFLAGS) -o xdump xdump.c

clean:
	rm -f $(PROGS)

install:
	strip $(PROGS)
	sudo install -m 0555 $(PROGS) /usr/local/bin
