#include "cilo/editor/input.h"

#include <ctype.h>
#include <stdlib.h>

#include <unistd.h>

#include <cilo/common.h>
#include <cilo/editor/operations.h>
#include <cilo/editor/row.h>
#include <cilo/editor/state.h>
#include <cilo/error.h>
#include <cilo/file_io.h>
#include <cilo/terminal.h>
#include <cilo/utils.h>

#define CTRL_PLUS(k) ((k) & (0x1f))

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
                    case '3': return KeyDelete;
                    case '5': return KeyPageUp;
                    case '6': return KeyPageDown;
                    case '1': case '7': return KeyHome;
                    case '4': case '8': return KeyEnd;
                }  // clang-format on
            }
        }
        else
        {
            switch (seq[1])  // clang-format off
            {
                case 'A': return KeyArrowUp;
                case 'B': return KeyArrowDown;
                case 'C': return KeyArrowRight;
                case 'D': return KeyArrowLeft;
                case 'H': return KeyHome;
                case 'F': return KeyEnd;
            }  // clang-format on
        }
    }
    else if (seq[0] == 'O')
    {
        switch (seq[1])  // clang-format off
        {
            case 'H': return KeyHome;
            case 'F': return KeyEnd;
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

static struct EditorRow* get_current_row()
{
    return (editor.cursor_y < editor.num_rows)
             ? &editor.rows[editor.cursor_y]
             : NULL;
}

static void snap_cursor_to_end()
{
    const struct EditorRow* cr = get_current_row();

    editor.cursor_x = MIN(editor.cursor_x,
                          cr ? cr->length : 0);
}

static void move_cursor(int key)
{
    const struct EditorRow* cr = get_current_row();

    switch (key)
    {
        case KeyArrowUp:
            if (editor.cursor_y != 0)
                editor.cursor_y--;
            break;

        case KeyArrowLeft:
            if (editor.cursor_x != 0)
                editor.cursor_x--;
            else if (editor.cursor_y > 0)
            {
                editor.cursor_y--;
                editor.cursor_x = editor.rows[editor.cursor_y].length;
            }
            break;

        case KeyArrowDown:
            if (editor.cursor_y < editor.num_rows)
                editor.cursor_y++;
            break;

        case KeyArrowRight:
            if (cr && editor.cursor_x < cr->length)
                editor.cursor_x++;
            else if (cr && editor.cursor_x == cr->length)
            {
                editor.cursor_y++;
                editor.cursor_x = 0;
            }
            break;
    }

    snap_cursor_to_end();
}

void editor_input_process()
{
    static int quit_times = CILO_QUIT_TIMES;

    const int c = read_key();

    switch (c)
    {
        case '\r':
            editor_op_insert_new_line();
            break;

        case CTRL_PLUS('q'):
            if (editor.is_dirty && quit_times - 1 > 0)
            {
                editor_state_set_status_msg(
                  "WARNING! File has unsaved changes. "
                  "Press Ctrl-Q %d more times to quit.",
                  --quit_times);
                return;
            }
            terminal_clear_screen();
            exit(EXIT_SUCCESS);

        case CTRL_PLUS('s'):
            file_io_save();
            break;

        case CTRL_PLUS('f'):
            editor_op_search();
            break;

        case KeyArrowUp:
        case KeyArrowLeft:
        case KeyArrowDown:
        case KeyArrowRight:
            move_cursor(c);
            break;

        case KeyPageUp:
        case KeyPageDown:
        {
            if (c == KeyPageUp)
            {
                editor.cursor_y = editor.row_offset;
            }
            else if (c == KeyPageDown)
            {
                editor.cursor_y = MIN(editor.row_offset + editor.screen_rows - 2,
                                      editor.num_rows);
            }

            int times = editor.screen_rows;
            while (times--)
            {
                move_cursor(c == KeyPageUp ? KeyArrowUp : KeyArrowDown);
            }
        }
        break;

        case KeyHome:
            editor.cursor_x = 0;
            break;

        case KeyEnd:
            if (editor.cursor_y < editor.num_rows)
                editor.cursor_x = editor.rows[editor.cursor_y].length;
            break;

        case KeyBackspace:
        case CTRL_PLUS('h'):
        case KeyDelete:
            if (c == KeyDelete)
                move_cursor(KeyArrowRight);
            editor_op_delete_char();
            break;

        case CTRL_PLUS('l'):
        case '\x1b':
            break;

        default:
            editor_op_insert_char(c);
    }

    quit_times = CILO_QUIT_TIMES;
}

char* editor_input_from_prompt(const char* prompt,
                               void (*callback)(const char*, int))
{
    static const size_t BUF_INITIAL_CAPACITY = 64;

    size_t buf_capacity = BUF_INITIAL_CAPACITY;
    size_t buf_length   = 0;
    char*  buf          = malloc(buf_capacity);
    buf[0]              = '\0';

    while (true)
    {
        editor_state_set_status_msg(prompt, buf);
        editor_state_redraw();

        const int c = read_key();

        if (c == KeyDelete || c == CTRL_PLUS('h') || c == KeyBackspace)
        {
            if (buf_length != 0)
                buf[--buf_length] = '\0';
        }
        else if (c == '\x1b')
        {
            editor_state_set_status_msg("");

            if (callback)
                callback(buf, c);

            free(buf);
            return NULL;
        }
        else if (c == '\r')
        {
            if (buf_length != 0)
            {
                editor_state_set_status_msg("");

                if (callback)
                    callback(buf, c);

                return buf;
            }
        }
        else if (!iscntrl(c) && c < 128)
        {
            if (buf_length + 1 > buf_capacity)
            {
                buf_capacity *= 2;

                char* new_buf = realloc(buf, buf_capacity);
                if (new_buf == NULL)
                    die("editor_input_from_prompt");

                buf = new_buf;
            }

            buf[buf_length++] = c;
            buf[buf_length]   = '\0';
        }

        if (callback)
            callback(buf, c);
    }
}
