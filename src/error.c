#include "cilo/error.h"

#include <stdio.h>
#include <stdlib.h>

#include <cilo/editor.h>
#include <cilo/terminal.h>

void die(const char* s)
{
    clear_screen();
    perror(s);
    exit(EXIT_FAILURE);
}
