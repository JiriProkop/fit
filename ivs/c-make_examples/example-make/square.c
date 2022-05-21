#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./square X");
        return 1;
    }

    double x = atof(argv[1]);
    printf("%g^2 = %g\n", x, square(x));

    return 0;
}
