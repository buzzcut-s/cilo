#include "cilo/error.h"

#include <stdio.h>
#include <stdlib.h>

void die(const char* s)
{
    perror(s);
    exit(EXIT_FAILURE);
}
