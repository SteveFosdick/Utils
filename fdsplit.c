#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char err_ropen[] = "fdsplit: unable to open '%s' for reading: %m\n";
static const char err_wopen[] = "fdsplit: unable to open '%s' for writing: %m\n";

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
        fputs("Usage: fdsplit [ -s sectors ] [ -t tracks ] <dsd> <side1> <side2>\n", stderr);
        status = 1;
    }
    else {
        const char *dsd_fn = argv[optind++];
        FILE *dsd_fp = fopen(dsd_fn, "rb");
        if (dsd_fp) {
            size_t size = sects * 256;
            char *track = malloc(size);
            if (track) {
                const char *s1_fn = argv[optind++];
                FILE *s1_fp = fopen(s1_fn, "wb");
                if (s1_fp) {
                    const char *s2_fn = argv[optind++];
                    FILE *s2_fp = fopen(s2_fn, "wb");
                    if (s2_fp) {
                        for (int i = 0; i < tracks; i++) {
                            if (fread(track, size, 1, dsd_fp) != 1)
                                break;
                            fwrite(track, size, 1, s1_fp);
                            if (fread(track, size, 1, dsd_fp) != 1)
                                break;
                            fwrite(track, size, 1, s2_fp);
                        }
                        fclose(s2_fp);
                    }
                    else {
                        fprintf(stderr, err_wopen, s2_fn);
                        status = 5;
                    }
                    fclose(s1_fp);
                }
                else {
                    fprintf(stderr, err_wopen, s1_fn);
                    status = 4;
                }
                fclose(dsd_fp);
            }
            else {
                fprintf(stderr, "fdsplit: unable to allocate track buffer: %m\n");
                status = 2;
            }
        }
        else {
            fprintf(stderr, err_ropen, dsd_fn);
            status = 3;
        }
    }
    return status;
}
