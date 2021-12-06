#include "cilo/terminal.h"

#include <termios.h>
#include <unistd.h>

void enable_raw_mode()
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);

    /* local modes - echoing off */
    raw.c_lflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
