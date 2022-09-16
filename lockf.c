#ifdef linux
#define _GNU_SOURCE
#define LOCK_NWAI F_OFD_SETLK
#define LOCK_WAIT F_OFD_SETLKW
#else
#define LOCK_NWAI F_SETLK
#define LOCK_WAIT F_SETLKW
#endif

#include <argp.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int lock_cmd = LOCK_WAIT;

static struct flock fl = {
    .l_type   = F_WRLCK,
    .l_whence = SEEK_SET,
    .l_start  = 0,
    .l_len    = 0,
    .l_pid    = 0
};

static error_t parser (int key, char *arg, struct argp_state *state)
{
    switch(key) {
        case 'l':
            fl.l_len = strtoul(arg, NULL, 0);
            break;
        case 'n':
            lock_cmd = LOCK_NWAI;
            break;
        case 'r':
            fl.l_type = F_RDLCK;
            break;
        case 's':
            fl.l_start = strtoul(arg, NULL, 0);
            break;
        case 'w':
            fl.l_type = F_WRLCK;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static const struct argp_option options[] = {
    { "length",   'l', "length",        0, "the length of the region to lock"          },
    { "nonblock", 'n', NULL,            0, "don't block waiting for the lock"          },
    { "read",     'r', NULL,            0, "request a read (shared) lock"              },
    { "start",    's', "file position", 0, "the start of the file region to be locked" },
    { "write",    'w', NULL,            0, "request a write (exclusive) lock"          },
    { 0 }
};

static const struct argp opt_parse = { options, parser };

int main(int argc, char **argv)
{
    int status, arg_index;
    argp_parse(&opt_parse, argc, argv, ARGP_NO_ARGS, &arg_index, NULL);
    if ((arg_index + 2) <= argc) {
        const char *fn = argv[arg_index++];
        int mode;
        const char *mode_desc;
        if (fl.l_type == F_WRLCK) {
            mode = O_RDWR;
            mode_desc = "read/write";
        }
        else {
            mode = O_RDONLY;
            mode_desc = "reading";
        }
        int fd = open(fn, mode);
        if (fd >= 0) {
            if (!fcntl(fd, LOCK_WAIT, &fl)) {
                pid_t pid = fork();
                if (pid == 0) {
                    argv += arg_index;
                    execvp(argv[0], argv);
                    fprintf(stderr, "lockf: unable to exec %s: %m\n", argv[0]);
                    exit(127);
                }
                else if (pid == -1) {
                    fprintf(stderr, "lockf: unable to fork: %m\n");
                    status = 4;
                }
                else {
                    int wstat;
                    while (waitpid(pid, &wstat, 0) == -1 && errno == EINTR)
                        ;
                    if (WIFSIGNALED(wstat)) {
                        const char *core = "";
                        if (WCOREDUMP(wstat))
                            core = "(core dumped)";
                        fprintf(stderr, "lockf: %s terminated by %s%s\n", argv[arg_index], strsignal(WTERMSIG(wstat)), core);
                        status = 4;
                    }
                    else
                        status = WEXITSTATUS(wstat);
                }
            }
            else {
                fprintf(stderr, "lockf: unable to lock file '%s': %m\n", fn);
                status = 3;
            }
            close(fd);
        }
        else {
            fprintf(stderr, "lockf: unable to open file '%s' for %s: %m\n", fn, mode_desc);
            status = 2;
        }
    }
    else {
        fputs("lockf: missing filename/command\n", stderr);
        status = 1;
    }
    return status;
}
