#include "cilo/editor.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <cilo/append_buffer.h>
#include <cilo/error.h>

#define CTRL_PLUS(k) ((k) & (0x1f))

static int get_window_size(uint16_t* out_rows, uint16_t* out_cols);

static const char* const ERASE_IN_DISPLAY_ALL = "\x1b[2J";
static const char* const CURSOR_POSITION_1_1  = "\x1b[H";

struct EditorState editor;

void init_editor()
{
    if (get_window_size(&editor.screen_rows, &editor.screen_cols) == -1)
        die("get_window_size");
}

static char read_keypress()
{
    char    c      = 0;
    ssize_t n_read = 0;

    while ((n_read = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (n_read == -1)
            die("read");
    }

    return c;
}

void process_keypress()
{
    const char c = read_keypress();

    switch (c)
    {
        case CTRL_PLUS('q'):
            clear_screen();
            exit(EXIT_SUCCESS);
    }
}

static void erase_display(struct AppendBuffer* ab)
{
    buffer_insert(ab, ERASE_IN_DISPLAY_ALL, 4);
}

static void reset_cursor(struct AppendBuffer* ab)
{
    buffer_insert(ab, CURSOR_POSITION_1_1, 3);
}

static void draw_rows(struct AppendBuffer* ab)
{
    for (int y = 0; y < editor.screen_rows; y++)
    {
        buffer_insert(ab, "~", 1);

        if (y < editor.screen_rows - 1)
            buffer_insert(ab, "\r\n", 2);
    }
}

void clear_screen()
{
    write(STDIN_FILENO, ERASE_IN_DISPLAY_ALL, 4);
    write(STDIN_FILENO, CURSOR_POSITION_1_1, 3);
}

void refresh_screen()
{
    struct AppendBuffer ab = BUFFER_INIT;

    erase_display(&ab);
    reset_cursor(&ab);
    draw_rows(&ab);
    reset_cursor(&ab);

    buffer_flush(&ab);
    buffer_free(&ab);
}

static int move_cursor_to_bottom_right()
{
    static const char* const CUF_999_CUD_999 = "\x1b[999C\x1b[999B";
    return write(STDOUT_FILENO, CUF_999_CUD_999, 12) == 12 ? 12
                                                           : -1;
}

static int write_active_position()
{
    static const char* const DSR_ACTIVE_POSITION = "\x1b[6n";
    return write(STDOUT_FILENO, DSR_ACTIVE_POSITION, 4) == 4 ? 4
                                                             : -1;
}

static void read_active_position(char (*buf)[32])
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

static int parse_active_position(const char buf[], uint16_t* out_rows, uint16_t* out_cols)
{
    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;

    if (sscanf(&buf[2], "%hu;%hu", out_rows, out_cols) != 2)
        return -1;

    return 0;
}

static int write_screen_size(uint16_t* out_rows, uint16_t* out_cols)
{
    if (write_active_position() == -1)
        return -1;

    char buf[32];
    read_active_position(&buf);

    return parse_active_position(buf, out_rows, out_cols);
}

static int get_window_size(uint16_t* out_rows, uint16_t* out_cols)
{
    struct winsize current;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &current) == -1
        || current.ws_col == 0 || current.ws_row == 0)
    {
        if (move_cursor_to_bottom_right() == -1)
            return -1;

        return write_screen_size(out_rows, out_cols);
    }

    *out_rows = current.ws_row;
    *out_cols = current.ws_col;

    return 0;
}
