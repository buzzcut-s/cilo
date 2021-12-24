#include "cilo/editor/state.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <cilo/ansi_escape.h>
#include <cilo/editor/row.h>
#include <cilo/error.h>
#include <cilo/string_buffer.h>
#include <cilo/utils.h>
#include <cilo/window_size.h>

struct EditorState editor;

void init_editor()
{
    static const int ROWS_INITIAL_CAPACITY = 16;

    editor.cursor_x = 0;
    editor.cursor_y = 0;

    editor.row_offset = 0;
    editor.col_offset = 0;

    editor.rows          = malloc(sizeof(struct EditorRow) * (ROWS_INITIAL_CAPACITY));
    editor.num_rows      = 0;
    editor.rows_capacity = ROWS_INITIAL_CAPACITY;

    if (get_window_size(&editor.screen_rows, &editor.screen_cols) == -1)
        die("get_window_size");
}

static void clear_line(struct StringBuffer* sb)
{
    sbuffer_insert(sb, ERASE_IN_LINE_TILL_END, 3);
}

static void insert_padding(struct StringBuffer* sb, size_t padding)
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
    static const char* const WELCOME = "cilo editor";

    const size_t length = MIN(strlen(WELCOME), editor.screen_cols);

    const size_t padding_till_centre =
      (editor.screen_cols - length) / 2;

    insert_padding(sb, padding_till_centre);
    sbuffer_insert(sb, WELCOME, length);
}

static void display_file(struct StringBuffer* sb, size_t row_idx)
{
    const size_t length = MIN(
      MAX(
        editor.rows[row_idx].line_length - editor.col_offset, 0),
      editor.screen_cols);

    sbuffer_insert(sb, &editor.rows[row_idx].line_chars[editor.col_offset], length);
}

static void draw_rows(struct StringBuffer* sb)
{
    for (size_t y = 0; y < editor.screen_rows; y++)
    {
        const size_t current_row = y + editor.row_offset;

        if (current_row < editor.num_rows)
        {
            display_file(sb, current_row);
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
    sbuffer_insert(sb, CURSOR_POSITION_1_1, 3);
}

static void hide_cursor(struct StringBuffer* sb)
{
    sbuffer_insert(sb, SET_MODE_CURSOR_HIDE, 6);
}

static void show_cursor(struct StringBuffer* sb)
{
    sbuffer_insert(sb, SET_MODE_CURSOR_SHOW, 6);
}

static void update_cursor(struct StringBuffer* sb)
{
    char buf[32];

    const int buf_len = snprintf(buf, sizeof(buf), CURSOR_POSITION_Y_X,
                                 (editor.cursor_y - editor.row_offset) + 1,
                                 (editor.cursor_x - editor.col_offset) + 1);

    sbuffer_insert(sb, buf, buf_len);
}

void update_scroll()
{
    if (editor.cursor_y < editor.row_offset)
        editor.row_offset = editor.cursor_y;

    if (editor.cursor_y >= editor.row_offset + editor.screen_rows)
        editor.row_offset = editor.cursor_y - editor.screen_rows + 1;

    if (editor.cursor_x < editor.col_offset)
        editor.col_offset = editor.cursor_x;

    if (editor.cursor_x >= editor.col_offset + editor.screen_cols)
        editor.col_offset = editor.cursor_x - editor.screen_cols + 1;
}

void redraw_editor()
{
    update_scroll();

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
    er_store_line(&editor.rows[editor.num_rows], line, length);
    editor.num_rows++;
}
