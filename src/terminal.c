#include "cilo/terminal.h"

#include <stdlib.h>

#include <termios.h>
#include <unistd.h>

static struct termios original_terminal_state;

static void disable_raw_mode()
{
    /* restore original terminal attributes */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal_state);
}

void enable_raw_mode()
{
    /* store original terminal attributes */
    tcgetattr(STDIN_FILENO, &original_terminal_state);

    if (atexit(disable_raw_mode) != 0)
        exit(EXIT_FAILURE);

    struct termios raw = original_terminal_state;

    /* input modes - no break, no CR to NL, no parity check, no strip char
     * disable start/stop output control (^S,^Q) */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);

    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);

    /* local modes - echoing off, canonical off, disable extended functions
     * disable signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
