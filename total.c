#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    double total = 0.0;
    if (argc == 1) {
        char line[80];
        while (fgets(line, sizeof(line), stdin))
            total += atof(line);
    }
    else
        while (--argc)
            total += atof(*++argv);
    printf("%g\n", total);
    return 0;
}
