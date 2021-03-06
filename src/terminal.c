#include "cilo/terminal.h"

#include <stdlib.h>

#include <termios.h>
#include <unistd.h>

#include <cilo/ansi_escape.h>
#include <cilo/editor/state.h>
#include <cilo/error.h>

static inline void store_original_terminal_state()
{
    if (tcgetattr(STDIN_FILENO, &editor.original_state) == -1)
        die("tcgetattr");
}

static inline void restore_original_terminal_state()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &editor.original_state) == -1)
        die("tcsetattr");
}

static inline void disable_raw_mode()
{
    restore_original_terminal_state();
}

void terminal_enable_raw_mode()
{
    store_original_terminal_state();

    if (atexit(disable_raw_mode) != 0)
        exit(EXIT_FAILURE);

    struct termios raw = editor.original_state;

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

    /* Return each byte as soon as there is any input */
    raw.c_cc[VMIN] = 0;

    /* 100 ms timeout (unit is tenths of a second) */
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

void terminal_clear_screen()
{
    write(STDIN_FILENO, ERASE_IN_DISPLAY_ALL, 4);
    write(STDIN_FILENO, CURSOR_POSITION_1_1, 3);
}
