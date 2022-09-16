#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    long total = 0.0;
    if (argc > 1) {
        while (--argc) {
            const char *n = *++argv;
            total += strtol(n, NULL, 10);
        }
    }
    else {
        long value;
        while (scanf("%ld", &value) == 1)
            total += value;
    }
    printf("%ld\n", total);
    return 0;
}
