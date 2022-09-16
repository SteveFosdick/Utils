#define _GNU_SOURCE
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <systemd/sd-journal.h>

#define BIG_BLOCK_SZ (128*1024)

static volatile int abort_flag = 0;
static volatile int prog_flag = 0;

static void set_abort_flag(int sig)
{
	abort_flag = sig;
}

static void set_prog_flag(int sig)
{
	prog_flag = sig;
}

static void log_msg(int priority, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    sd_journal_printv(priority, format, ap);
    va_end(ap);
    fputs("sdtest: ", stderr);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    putc('\n', stderr);
}

static ssize_t read_at(int dfd, void *buf, size_t count, off_t posn)
{
	if (lseek(dfd, posn, SEEK_SET) != posn)
		return -1;
	return read(dfd, buf, count);
}

static ssize_t write_at(int dfd, void *buf, size_t count, off_t posn)
{
	if (lseek(dfd, posn, SEEK_SET) != posn)
		return -1;
	return write(dfd, buf, count);
}

static int sdtest(const char *dev, unsigned char *buf_save, unsigned char *buf_pat, unsigned char *buf_cmp)
{
	int status = 0;
	int dfd = open(dev, O_RDWR|O_DSYNC);
	if (dfd >= 0) {
		off_t bytesize = lseek(dfd, 0, SEEK_END);
		if (bytesize > 0) {
			off_t posn = 0;
			off_t bufs = (bytesize + BIG_BLOCK_SZ - 1) / BIG_BLOCK_SZ;
			for (off_t bufno = 0; !abort_flag && bufno < bufs && status == 0; bufno++) {
				if (prog_flag) {
					printf("block %'lu of %'lu %lu%%\n", bufno, bufs, (bufno * 100) / bufs);
					prog_flag = 0;
				}
				ssize_t nbytes = read_at(dfd, buf_save, BIG_BLOCK_SZ, posn);
				if (nbytes <= 0) {
					log_msg(LOG_ERR, "unable to read %s for backup at %lu: %m", dev, posn);
					status = 5;
				}
				else {
					off_t val = bufno;
					for (unsigned char *ptr = buf_pat; ptr < buf_cmp; )
						*ptr++ = val++;
					if (write_at(dfd, buf_pat, nbytes, posn) != nbytes) {
						log_msg(LOG_ERR, "unable to write pattern to %s at %lu: %m", dev, posn);
						status = 6;
					}
					else if (ioctl(dfd, BLKFLSBUF, NULL)) {
						log_msg(LOG_ERR, "unable to flush block cache for %s at %lu: %m", dev, posn);
						status = 7;
					}
					else if (read_at(dfd, buf_cmp, nbytes, posn) != nbytes) {
						log_msg(LOG_ERR, "unable to read pattern from %s at %lu: %m", dev, posn);
						status = 8;
					}
					else if (memcmp(buf_pat, buf_cmp, nbytes)) {
						log_msg(LOG_ERR, "pattern compare failed on %s at %lu: %m", dev, posn);
						status = 9;
					}
					if (write_at(dfd, buf_save, nbytes, posn) != nbytes) {
						log_msg(LOG_ERR, "unable to write backup to %s at %lu: %m", dev, posn);
						if (!status)
							status = 10;
					}
				}
			} /* for each block */
			if (abort_flag) {
				log_msg(LOG_NOTICE, "aborted by %s while testing %s", strsignal(abort_flag), dev);
				status = 11;
			}
		}
		else {
			log_msg(LOG_ERR, "invalid size for %s: %m", dev);
			status = 4;
		}
		close(dfd);
	}
	else {
		log_msg(LOG_ERR, "unable to open %s: %m", dev);
		status = 3;
	}
	return status;
}

int main(int argc, char **argv)
{
	int status = 0;
	setlocale(LC_ALL, "");
	if (--argc) {
		unsigned char *buf_save = malloc(3*BIG_BLOCK_SZ);
		if (buf_save) {
			struct sigaction sa;
			sa.sa_handler = SIG_IGN;
			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			if (!sigaction(SIGHUP, &sa, NULL) && !sigaction(SIGPIPE, &sa, NULL) && !sigaction(SIGTTOU, &sa, NULL)) {
				sa.sa_handler = set_abort_flag;
				if (!sigaction(SIGQUIT, &sa, NULL) && !sigaction(SIGTERM, &sa, NULL)) {
					sa.sa_handler = set_prog_flag;
					if (!sigaction(SIGINT, &sa, NULL)) {
						unsigned char *buf_pat = buf_save + BIG_BLOCK_SZ;
						unsigned char *buf_cmp = buf_pat + BIG_BLOCK_SZ;
						do
							status = sdtest(*++argv, buf_save, buf_pat, buf_cmp);
						while (!status && --argc);
					}
					else {
						log_msg(LOG_ERR, "unable to set signal handler: %m");
						status = 3;
					}
				}
				else {
					log_msg(LOG_ERR, "unable to set signal handler: %m");
					status = 3;
				}
			}
			else {
				log_msg(LOG_ERR, "unable to set signal handler: %m");
				status = 3;
			}
		}
		else {
			log_msg(LOG_ERR, "sdtest: out of memory");
			status = 2;
		}
	}
	else {
		fputs("Usage: sdtest <dev> [ <dev> ... ]\n", stderr);
		status = 1;
	}
	return status;
}
