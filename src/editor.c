#include "cilo/editor.h"

#include <stdlib.h>

#include <unistd.h>

#include <cilo/error.h>

#define CTRL_PLUS(k) ((k) & (0x1f))

static char read_keypress()
{
    char c = 0;

    if (read(STDIN_FILENO, &c, 1) == -1)
        die("read");

    return c;
}

void process_keypress()
{
    const char c = read_keypress();

    switch (c)
    {
        case CTRL_PLUS('q'):
            exit(EXIT_SUCCESS);
    }
}
