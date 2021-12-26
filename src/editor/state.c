#include "cilo/editor/state.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <cilo/ansi_escape.h>
#include <cilo/common.h>
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

    editor.render_x = 0;

    editor.row_offset = 0;
    editor.col_offset = 0;

    editor.rows          = malloc(sizeof(struct EditorRow) * (ROWS_INITIAL_CAPACITY));
    editor.num_rows      = 0;
    editor.rows_capacity = ROWS_INITIAL_CAPACITY;

    editor.filename = NULL;

    editor.help_msg_time = 0;
    editor.help_msg[0]   = '\0';

    if (get_window_size(&editor.screen_rows, &editor.screen_cols) == -1)
        die("get_window_size");

    editor.screen_rows -= 2;
}

static void clear_line(struct StringBuffer* sb)
{
    sbuffer_insert(sb, ERASE_IN_LINE_TILL_END, 3);
}

static void insert_padding(struct StringBuffer* sb, size_t padding)
{
    if (padding--)
        sbuffer_insert(sb, "~", 1);

    while (padding--)
        sbuffer_insert(sb, " ", 1);
}

static void display_welcome(struct StringBuffer* sb)
{
    static const char* const WELCOME = "cilo editor";

    const size_t length = MIN(strlen(WELCOME), editor.screen_cols);

    const size_t padding_till_centre = (editor.screen_cols - length) / 2;

    insert_padding(sb, padding_till_centre);
    sbuffer_insert(sb, WELCOME, length);
}

static void display_file(struct StringBuffer* sb, size_t row_idx)
{
    const int64_t chars_to_right = editor.rows[row_idx].render_length - editor.col_offset;

    const size_t length = MIN(MAX(chars_to_right, 0), editor.screen_cols);

    sbuffer_insert(sb, &editor.rows[row_idx].render_chars[editor.col_offset], length);
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
                                 (editor.render_x - editor.col_offset) + 1);

    sbuffer_insert(sb, buf, buf_len);
}

static size_t editor_cx_to_rx(struct EditorRow* row, size_t cursor_x)
{
    size_t rx = 0;
    for (size_t i = 0; i < cursor_x; i++)
    {
        if (row->line_chars[i] == '\t')
            rx += (CILO_TAB_STOP - 1) - (rx % CILO_TAB_STOP);
        rx++;
    }

    return rx;
}

void update_scroll()
{
    editor.render_x = 0;
    if (editor.cursor_y < editor.num_rows)
        editor.render_x = editor_cx_to_rx(&editor.rows[editor.cursor_y], editor.cursor_x);

    if (editor.cursor_y < editor.row_offset)
        editor.row_offset = editor.cursor_y;

    if (editor.cursor_y >= editor.row_offset + editor.screen_rows)
        editor.row_offset = editor.cursor_y - editor.screen_rows + 1;

    if (editor.render_x < editor.col_offset)
        editor.col_offset = editor.render_x;

    if (editor.render_x >= editor.col_offset + editor.screen_cols)
        editor.col_offset = editor.render_x - editor.screen_cols + 1;
}

static void draw_status_bar(struct StringBuffer* sb)
{
    sbuffer_insert(sb, GRAPHIC_RENDITION_INVERTED, 4);

    char left_status[80];
    int  left_len = snprintf(left_status, sizeof(left_status), "%.20s - %zu lines",
                            editor.filename ? editor.filename : "[No Name]",
                             editor.num_rows);

    left_len = MIN(left_len, editor.screen_cols);
    sbuffer_insert(sb, left_status, left_len);

    char      right_status[80];
    const int right_len = snprintf(right_status, sizeof(right_status), "%zu/%zu",
                                   editor.cursor_y + 1,
                                   editor.num_rows);

    while (left_len < editor.screen_cols)
    {
        if (editor.screen_cols - left_len == right_len)
        {
            sbuffer_insert(sb, right_status, right_len);
            break;
        }

        sbuffer_insert(sb, " ", 1);
        left_len++;
    }

    sbuffer_insert(sb, GRAPHIC_RENDITION_NORMAL, 3);
}

static void draw_message_bar(struct StringBuffer* sb)
{
    static const int HELP_MSG_TIMEOUT = 5;

    sbuffer_insert(sb, "\r\n", 2);
    clear_line(sb);

    const size_t msg_len = MIN(strlen(editor.help_msg), editor.screen_cols);

    if (msg_len && (time(NULL) - editor.help_msg_time < HELP_MSG_TIMEOUT))
        sbuffer_insert(sb, editor.help_msg, msg_len);
}

void redraw_editor()
{
    update_scroll();

    struct StringBuffer sb = BUFFER_ZERO;
    sbuffer_init(&sb);

    hide_cursor(&sb);
    reset_cursor(&sb);

    draw_rows(&sb);
    draw_status_bar(&sb);
    draw_message_bar(&sb);

    update_cursor(&sb);
    show_cursor(&sb);

    sbuffer_flush(&sb);
    sbuffer_free(&sb);
}

static void update_render(struct EditorRow* row)
{
    size_t n_tabs = 0;

    for (size_t i = 0; i < row->line_length; i++)
    {
        if (row->line_chars[i] == '\t')
            n_tabs++;
    }

    row->render_chars = malloc(row->line_length + n_tabs * (CILO_TAB_STOP - 1) + 1);
    if (row->render_chars == NULL)
        die("update_render");

    size_t idx = 0;
    for (size_t j = 0; j < row->line_length; j++)
    {
        if (row->line_chars[j] == '\t')
        {
            row->render_chars[idx++] = ' ';
            while (idx % CILO_TAB_STOP != 0)
                row->render_chars[idx++] = ' ';
        }
        else
        {
            row->render_chars[idx++] = row->line_chars[j];
        }
    }

    row->render_length     = idx;
    row->render_chars[idx] = '\0';
}

void store_line(const char* line, size_t length)
{
    if (editor.num_rows + 1 > editor.rows_capacity)
    {
        editor.rows_capacity *= 2;

        struct EditorRow* new_rows = realloc(
          editor.rows, sizeof(struct EditorRow) * (editor.rows_capacity));
        if (new_rows == NULL)
            die("store_line");

        editor.rows = new_rows;
    }

    er_store_line(&editor.rows[editor.num_rows], line, length);

    update_render(&editor.rows[editor.num_rows]);

    editor.num_rows++;
}

void editor_set_help_message(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsnprintf(editor.help_msg, sizeof(editor.help_msg), format, ap);
    va_end(ap);

    editor.help_msg_time = time(NULL);
}
