#include "cilo/editor/state.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <cilo/error.h>
#include <cilo/string_buffer.h>
#include <cilo/window_size.h>

struct EditorState editor;

void init_editor()
{
    editor.cursor_x = 0;
    editor.cursor_y = 0;

    if (get_window_size(&editor.screen_rows, &editor.screen_cols) == -1)
        die("get_window_size");
}

static void clear_line(struct StringBuffer* sb)
{
    static const char* const ERASE_IN_LINE_TILL_END = "\x1b[K";
    sbuffer_insert(sb, ERASE_IN_LINE_TILL_END, 3);
}

static void insert_padding(struct StringBuffer* sb, uint64_t padding)
{
    if (padding)
    {
        sbuffer_insert(sb, "~", 1);
        padding--;
    }

    while (padding--)
        sbuffer_insert(sb, " ", 1);
}

static void print_welcome(struct StringBuffer* sb)
{
    const char* welcome = "cilo editor";

    uint64_t welcome_length = strlen(welcome);
    if (welcome_length > editor.screen_cols)
        welcome_length = editor.screen_cols;

    const uint64_t padding_till_centre =
      (editor.screen_cols - welcome_length) / 2;

    insert_padding(sb, padding_till_centre);

    sbuffer_insert(sb, welcome, welcome_length);
}

static void draw_rows(struct StringBuffer* sb)
{
    for (int y = 0; y < editor.screen_rows; y++)
    {
        if (y == editor.screen_rows / 3)
            print_welcome(sb);
        else
            sbuffer_insert(sb, "~", 1);

        clear_line(sb);
        if (y < editor.screen_rows - 1)
            sbuffer_insert(sb, "\r\n", 2);
    }
}

static void reset_cursor(struct StringBuffer* sb)
{
    static const char* const CURSOR_POSITION_1_1 = "\x1b[H";
    sbuffer_insert(sb, CURSOR_POSITION_1_1, 3);
}

static void hide_cursor(struct StringBuffer* sb)
{
    static const char* const SET_MODE_CURSOR_HIDE = "\x1b[?25l";
    sbuffer_insert(sb, SET_MODE_CURSOR_HIDE, 6);
}

static void show_cursor(struct StringBuffer* sb)
{
    static const char* const SET_MODE_CURSOR_SHOW = "\x1b[?25h";
    sbuffer_insert(sb, SET_MODE_CURSOR_SHOW, 6);
}

static void update_cursor(struct StringBuffer* sb)
{
    static const char* const CURSOR_POSITION_Y_X = "\x1b[%d;%dH";

    char buf[32];

    const int buf_len = snprintf(buf, sizeof(buf), CURSOR_POSITION_Y_X,
                                 editor.cursor_y + 1, editor.cursor_x + 1);

    sbuffer_insert(sb, buf, buf_len);
}

void redraw_editor()
{
    struct StringBuffer sb = BUFFER_ZERO;
    sbuffer_init(&sb);

    hide_cursor(&sb);
    reset_cursor(&sb);

    draw_rows(&sb);

    update_cursor(&sb);
    show_cursor(&sb);

    sbuffer_flush(&sb);
    sbuffer_free(&sb);
}
