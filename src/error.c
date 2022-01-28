#include "cilo/error.h"

#include <stdio.h>
#include <stdlib.h>

#include <cilo/terminal.h>

void die(const char* s)
{
    terminal_clear_screen();
    perror(s);
    exit(EXIT_FAILURE);
}
