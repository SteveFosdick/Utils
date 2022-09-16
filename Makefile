CFLAGS =-O2 -Wall
LDFLAGS = -O2
PROGS  = txt2bbc txt2dos txt2ux txt2cpm txt2bas bas2txt rmnl xdump fdcombine fdsplit sdtest lockf clean7 total

all: $(PROGS)

txt2bbc: txt2bbc.c txtconv.c
	$(CC) $(CFLAGS) -o txt2bbc txt2bbc.c

txt2dos: txt2dos.c txtconv.c
	$(CC) $(CFLAGS) -o txt2dos txt2dos.c

txt2cpm: txt2cpm.c txtconv.c
	$(CC) $(CFLAGS) -o txt2cpm txt2cpm.c

txt2ux: txt2ux.c txtconv.c
	$(CC) $(CFLAGS) -o txt2ux txt2ux.c

rmnl: rmnl.c txtconv.c
	$(CC) $(CFLAGS) -o rmnl rmnl.c

xdump: xdump.c filter.c
	$(CC) $(CFLAGS) -o xdump xdump.c

sdtest: sdtest.c
	$(CC) $(CFLAGS) -o sdtest sdtest.c `pkg-config --libs libsystemd`

lockf: lockf.c
	$(CC) $(CFLAGS) -o lockf lockf.c

clean7: clean7.c filter.c
	$(CC) $(CFLAGS) -o clean7 clean7.c

total: total.c
	$(CC) $(CFLAGS) -o total total.c

clean:
	rm -f $(PROGS)

install:
	strip $(PROGS)
	sudo install -m 0555 $(PROGS) /usr/local/bin
