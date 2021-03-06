#include "cilo/window_size.h"

#include <stdio.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <cilo/ansi_escape.h>

static inline int move_cursor_to_bottom_right()
{
    return write(STDOUT_FILENO, CURSOR_F999_D999, 12) == 12 ? 12
                                                            : -1;
}

static inline int write_active_position()
{
    return write(STDOUT_FILENO, DSR_ACTIVE_POSITION, 4) == 4 ? 4
                                                             : -1;
}

static inline void read_active_position(char (*buf)[32])
{
    unsigned int i = 0;
    while (i < sizeof(*buf) - 1)
    {
        if (read(STDIN_FILENO, &(*buf)[i], 1) != 1)
            break;

        if ((*buf)[i] == 'R')
            break;

        i++;
    }
    (*buf)[i] = '\0';
}

static inline int parse_active_position(const char buf[], uint16_t* out_rows, uint16_t* out_cols)
{
    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;

    if (sscanf(&buf[2], "%hu;%hu", out_rows, out_cols) != 2)
        return -1;

    return 0;
}

static inline int write_screen_size(uint16_t* out_rows, uint16_t* out_cols)
{
    if (move_cursor_to_bottom_right() == -1)
        return -1;
    if (write_active_position() == -1)
        return -1;

    char buf[32];
    read_active_position(&buf);

    return parse_active_position(buf, out_rows, out_cols);
}

int get_window_size(uint16_t* out_rows, uint16_t* out_cols)
{
    struct winsize current;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &current) == -1
        || current.ws_col == 0
        || current.ws_row == 0)
    {
        return write_screen_size(out_rows, out_cols);
    }

    *out_rows = current.ws_row;
    *out_cols = current.ws_col;

    return 0;
}
