#include "cilo/editor.h"

#include <stdint.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <cilo/error.h>

#define CTRL_PLUS(k) ((k) & (0x1f))

static int get_window_size(uint16_t* out_rows, uint16_t* out_cols);

struct EditorState editor;

void init_editor()
{
    if (get_window_size(&editor.screen_rows, &editor.screen_cols))
        die("get_window_size");
}

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
            clear_screen();
            exit(EXIT_SUCCESS);
    }
}

static void draw_rows()
{
    for (int y = 0; y < editor.screen_rows; y++)
    {
        write(STDERR_FILENO, "~\r\n", 3);
    }
}

static void erase_display()
{
    static const char* const ERASE_IN_DISPLAY_ALL = "\x1b[2J";
    write(STDIN_FILENO, ERASE_IN_DISPLAY_ALL, 4);
}

static void reset_cursor()
{
    static const char* const CURSOR_POSITION_1_1 = "\x1b[H";
    write(STDIN_FILENO, CURSOR_POSITION_1_1, 3);
}

void clear_screen()
{
    erase_display();
    reset_cursor();
}

void refresh_screen()
{
    clear_screen();
    draw_rows();
    reset_cursor();
}

static int get_window_size(uint16_t* out_rows, uint16_t* out_cols)
{
    struct winsize current;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &current) == -1
        || current.ws_col == 0
        || current.ws_row == 0)
    {
        return -1;
    }

    *out_rows = current.ws_row;
    *out_cols = current.ws_col;

    return 0;
}
