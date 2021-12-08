#include "cilo/editor/state.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <cilo/editor/row.h>
#include <cilo/error.h>
#include <cilo/string_buffer.h>
#include <cilo/window_size.h>

struct EditorState editor;

void init_editor()
{
    static const int ROWS_INITIAL_CAPACITY = 16;

    editor.cursor_x = 0;
    editor.cursor_y = 0;

    editor.rows          = malloc(sizeof(struct EditorRow) * (ROWS_INITIAL_CAPACITY));
    editor.num_rows      = 0;
    editor.rows_capacity = ROWS_INITIAL_CAPACITY;

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

static void display_welcome(struct StringBuffer* sb)
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

static void display_file(struct StringBuffer* sb, int row_idx)
{
    size_t length = editor.rows[row_idx].length;
    if (length > editor.screen_cols)
        length = editor.screen_cols;

    sbuffer_insert(sb, editor.rows[row_idx].line, length);
}

static void draw_rows(struct StringBuffer* sb)
{
    for (int y = 0; y < editor.screen_rows; y++)
    {
        if (y < editor.num_rows)
        {
            display_file(sb, y);
        }
        else
        {
            if (editor.num_rows == 0 && y == editor.screen_rows / 3)
                display_welcome(sb);
            else
                sbuffer_insert(sb, "~", 1);
        }

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

static void grow_rows_array()
{
    if (editor.num_rows + 1 > editor.rows_capacity)
    {
        editor.rows_capacity *= 2;
        editor.rows = realloc(editor.rows,
                              sizeof(struct EditorRow) * (editor.rows_capacity));
    }
}

void store_line(const char* line, size_t length)
{
    grow_rows_array();
    er_write_line(&editor.rows[editor.num_rows], line, length);
    editor.num_rows++;
}
