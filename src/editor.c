#include "cilo/editor.h"

#include <stdlib.h>

#include <unistd.h>

#include <cilo/error.h>

#define CTRL_PLUS(k) ((k) & (0x1f))

struct EditorState editor;

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
    for (int y = 0; y < 24; y++)
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
