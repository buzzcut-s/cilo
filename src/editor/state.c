#include "cilo/editor/state.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <cilo/ansi_escape.h>
#include <cilo/common.h>
#include <cilo/editor/highlight.h>
#include <cilo/editor/row.h>
#include <cilo/error.h>
#include <cilo/string_buffer.h>
#include <cilo/utils.h>
#include <cilo/window_size.h>

struct EditorState editor;

void editor_state_init()
{
    static const int ROWS_INITIAL_CAPACITY = 16;

    editor.is_dirty = false;

    editor.cursor_x = 0;
    editor.cursor_y = 0;

    editor.render_x = 0;

    editor.row_offset = 0;
    editor.col_offset = 0;

    editor.rows = malloc(sizeof(struct EditorRow) * (ROWS_INITIAL_CAPACITY));
    if (editor.rows == NULL)
        die("editor_state_init");

    editor.num_rows      = 0;
    editor.rows_capacity = ROWS_INITIAL_CAPACITY;

    editor.filename = NULL;

    editor.status_msg_time = 0;
    editor.status_msg[0]   = '\0';

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
    {
        sbuffer_insert(sb, " ", 1);
    }
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

    const char*    c  = &editor.rows[row_idx].render_chars[editor.col_offset];
    const uint8_t* hl = &editor.rows[row_idx].highlight[editor.col_offset];

    int prev_color = HighlightDefault;
    for (size_t i = 0; i < length; i++)
    {
        if (hl[i] == HighlightNormal)
        {
            if (prev_color != HighlightDefault)
            {
                prev_color = HighlightDefault;
                sbuffer_insert(sb, "\x1b[39m", 5);
            }
            sbuffer_insert(sb, &c[i], 1);
        }
        else
        {
            const int color = eh_highlight_to_color(hl[i]);
            if (color != prev_color)
            {
                prev_color = color;

                char      color_buf[16];
                const int color_len = snprintf(color_buf, sizeof(color_buf),
                                               "\x1b[%dm", color);

                sbuffer_insert(sb, color_buf, color_len);
            }
            sbuffer_insert(sb, &c[i], 1);
        }
    }

    sbuffer_insert(sb, "\x1b[39m", 5);
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

static size_t editor_cx_to_rx(const struct EditorRow* row, size_t cx)
{
    size_t rx = 0;
    for (size_t i = 0; i < cx; i++)
    {
        if (row->chars[i] == '\t')
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
    int  left_len = snprintf(left_status, sizeof(left_status), "%.20s - %zu lines %s",
                            editor.filename ? editor.filename : "[No Name]",
                             editor.num_rows,
                            editor.is_dirty ? "(modified)" : "");

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
    static const int STATUS_MSG_TIMEOUT = 5;

    sbuffer_insert(sb, "\r\n", 2);
    clear_line(sb);

    const size_t msg_len = MIN(strlen(editor.status_msg), editor.screen_cols);

    if (msg_len && (time(NULL) - editor.status_msg_time < STATUS_MSG_TIMEOUT))
        sbuffer_insert(sb, editor.status_msg, msg_len);
}

void editor_state_redraw()
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

void editor_state_insert_line(size_t at, const char* line, size_t length)
{
    if (editor.num_rows + 1 > editor.rows_capacity)
    {
        editor.rows_capacity *= 2;

        struct EditorRow* new_rows = realloc(editor.rows, sizeof(struct EditorRow)
                                                            * (editor.rows_capacity));
        if (new_rows == NULL)
            die("editor_state_insert_line");

        editor.rows = new_rows;
    }

    memmove(&editor.rows[at + 1], &editor.rows[at],
            sizeof(struct EditorRow) * (editor.num_rows - at));

    er_store_line(&editor.rows[at], line, length);
    er_update_render(&editor.rows[at]);

    editor.num_rows++;
    editor.is_dirty = true;
}

void editor_state_delete_line(size_t at)
{
    er_free(&editor.rows[at]);
    memmove(&editor.rows[at], &editor.rows[at + 1],
            sizeof(struct EditorRow) * (editor.num_rows - at - 1));

    editor.num_rows--;
    editor.is_dirty = true;
}

void editor_state_set_status_msg(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsnprintf(editor.status_msg, sizeof(editor.status_msg), format, ap);
    va_end(ap);

    editor.status_msg_time = time(NULL);
}
