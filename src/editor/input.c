#include "cilo/editor/input.h"

#include <stdlib.h>

#include <unistd.h>

#include <cilo/editor/row.h>
#include <cilo/editor/state.h>
#include <cilo/error.h>
#include <cilo/terminal.h>
#include <cilo/utils.h>

#define CTRL_PLUS(k) ((k) & (0x1f))

enum EditorKey
{
    ArrowUp = 2103,
    ArrowLeft,
    ArrowDown,
    ArrowRight,
    PageUp,
    PageDown,
    Home,
    End,
};

static int handle_escape_sequence()
{
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
        return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
        return '\x1b';

    if (seq[0] == '[')
    {
        if (seq[1] >= '0' && seq[1] <= '9')
        {
            if (read(STDIN_FILENO, &seq[2], 1) != 1)
                return '\x1b';

            if (seq[2] == '~')
            {
                switch (seq[1])  // clang-format off
                {
                    case '5': return PageUp;
                    case '6': return PageDown;
                    case '1': case '7': return Home;
                    case '4': case '8': return End;
                }  // clang-format on
            }
        }
        else
        {
            switch (seq[1])  // clang-format off
            {
                case 'A': return ArrowUp;
                case 'B': return ArrowDown;
                case 'C': return ArrowRight;
                case 'D': return ArrowLeft;
                case 'H': return Home;
                case 'F': return End;
            }  // clang-format on
        }
    }
    else if (seq[0] == 'O')
    {
        switch (seq[1])  // clang-format off
        {
            case 'H': return Home;
            case 'F': return End;
        }  // clang-format on
    }

    return '\x1b';
}

static int read_key()
{
    char    c      = 0;
    ssize_t n_read = 0;

    while ((n_read = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (n_read == -1)
            die("read");
    }

    if (c == '\x1b')
        return handle_escape_sequence();

    return c;
}

static void move_cursor(int key)
{
    struct EditorRow* current_row = (editor.cursor_y < editor.num_rows)
                                      ? &editor.rows[editor.cursor_y]
                                      : NULL;
    switch (key)
    {
        case ArrowUp:
            if (editor.cursor_y != 0)
                editor.cursor_y--;
            break;

        case ArrowLeft:
            if (editor.cursor_x != 0)
                editor.cursor_x--;
            break;

        case ArrowDown:
            if (editor.cursor_y < editor.num_rows)
                editor.cursor_y++;
            break;

        case ArrowRight:
            if (current_row && editor.cursor_x < current_row->length)
                editor.cursor_x++;
            break;
    }

    current_row = (editor.cursor_y < editor.num_rows)
                    ? &editor.rows[editor.cursor_y]
                    : NULL;

    editor.cursor_x = MIN(editor.cursor_x,
                          current_row ? current_row->length : 0);
}

void process_keypress()
{
    const int c = read_key();

    switch (c)
    {
        case CTRL_PLUS('q'):
            clear_screen();
            exit(EXIT_SUCCESS);

        case ArrowUp:
        case ArrowLeft:
        case ArrowDown:
        case ArrowRight:
            move_cursor(c);
            break;

        case PageUp:
        case PageDown:
        {
            int times = editor.screen_rows;
            while (times--)
                move_cursor(c == PageUp ? ArrowUp : ArrowDown);
        }
        break;

        case Home:
            editor.cursor_x = 0;
            break;

        case End:
            editor.cursor_x = editor.screen_cols - 1;
            break;
    }
}