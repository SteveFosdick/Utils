#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char err_ropen[] = "fdcombine: unable to open '%s' for reading: %m\n";
static const char err_wopen[] = "fdcombine: unable to open '%s' for writing: %m\n";
static const char err_read[]  = "fdcombine: error reading %s: %m\n";

static int copy_track(char *track, size_t size, FILE *src_fp, const char *src_fn, int src_eof, FILE *dst_fp)
{
    if (src_eof)
        fseek(dst_fp, size, SEEK_CUR);
    else if (fread(track, size, 1, src_fp) == 1)
        fwrite(track, size, 1, dst_fp);
    else if (ferror(src_fp))
        fprintf(stderr, err_read, src_fn);
    else
        src_eof = 1;
    return src_eof;
}

int main(int argc, char * const argv[])
{
    int opt, status = 0, sects = 10, tracks = 80;

    while ((opt = getopt(argc, argv, "s:t:")) != EOF) {
        switch(opt) {
            case 's':
                sects = atoi(optarg);
                break;
            case 't':
                tracks = atoi(optarg);
                break;
            default:
                status = 1;
        }
    }
    if (status || (argc - optind) != 3) {
        fputs("Usage: fdcombine [ -s sectors ] [ -t tracks ] <side1> <side2> <dsd>\n", stderr);
        status = 1;
    }
    else {
        const char *s1_fn = argv[optind++];
        FILE *s1_fp = fopen(s1_fn, "rb");
        if (s1_fp) {
            const char *s2_fn = argv[optind++];
            FILE *s2_fp = fopen(s2_fn, "rb");
            if (s2_fp) {
                size_t size = sects * 256;
                char *track = malloc(size);
                if (track) {
                    const char *dsd_fn = argv[optind++];
                    FILE *dsd_fp = fopen(dsd_fn, "wb");
                    if (dsd_fp) {
                        int s1_eof = 0, s2_eof = 0;
                        for (int i = 0; i < tracks; i++) {
                            s1_eof = copy_track(track, size, s1_fp, s1_fn, s1_eof, dsd_fp);
                            s2_eof = copy_track(track, size, s2_fp, s2_fn, s2_eof, dsd_fp);
                        }
                        fclose(dsd_fp);
                    }
                    else {
                        fprintf(stderr, err_wopen, dsd_fn);
                        status = 3;
                    }
                }
                else {
                    fprintf(stderr, "fdcombine: unable to allocate track buffer: %m\n");
                    status = 2;
                }
                fclose(s2_fp);
            }
            else {
                fprintf(stderr, err_ropen, s2_fn);
                status = 5;
            }
            fclose(s1_fp);
        }
        else {
            fprintf(stderr, err_ropen, s1_fn);
            status = 4;
        }
    }
    return status;
}
