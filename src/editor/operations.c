#include "cilo/editor/operations.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <cilo/common.h>
#include <cilo/editor/input.h>
#include <cilo/editor/row.h>
#include <cilo/editor/state.h>

void editor_op_insert_char(int c)
{
    if (editor.cursor_y == editor.num_rows)
        editor_state_insert_line(editor.num_rows, "", 0);

    er_insert_character(&editor.rows[editor.cursor_y], editor.cursor_x, c);
    editor.is_dirty = true;

    editor.cursor_x++;
}

void editor_op_delete_char()
{
    if (editor.cursor_y == editor.num_rows)
        return;
    if (editor.cursor_x == 0 && editor.cursor_y == 0)
        return;

    struct EditorRow* row = &editor.rows[editor.cursor_y];
    if (editor.cursor_x > 0)
    {
        er_delete_character(row, editor.cursor_x - 1);
        editor.cursor_x--;
    }
    else
    {
        editor.cursor_x = editor.rows[editor.cursor_y - 1].length;
        er_append_string(&editor.rows[editor.cursor_y - 1], row->chars, row->length);
        editor_state_delete_line(editor.cursor_y);
        editor.cursor_y--;
    }

    editor.is_dirty = true;
}

void editor_op_insert_new_line()
{
    if (editor.cursor_x == 0)
    {
        editor_state_insert_line(editor.cursor_y, "", 0);
    }
    else
    {
        struct EditorRow* row = &editor.rows[editor.cursor_y];

        editor_state_insert_line(editor.cursor_y + 1, &row->chars[editor.cursor_x],
                                 row->length - editor.cursor_x);

        row = &editor.rows[editor.cursor_y];

        row->length             = editor.cursor_x;
        row->chars[row->length] = '\0';

        er_update_render(row);
    }

    editor.cursor_y++;
    editor.cursor_x = 0;
}

static size_t editor_rx_to_cx(const struct EditorRow* row, size_t rx)
{
    size_t curr_rx = 0;

    size_t cx = 0;
    for (cx = 0; cx < row->length; cx++)
    {
        if (row->chars[cx] == '\t')
            curr_rx += (CILO_TAB_STOP - 1) - (curr_rx % CILO_TAB_STOP);
        curr_rx++;

        if (curr_rx > rx)
            return cx;
    }
    return cx;
}

static void op_search_callback(const char* query, int key)
{
    if (key == '\r' || key == '\x1b')
        return;

    for (size_t i = 0; i < editor.num_rows; i++)
    {
        const struct EditorRow* row = &editor.rows[i];

        const char* matched = strstr(row->render_chars, query);
        if (matched)
        {
            editor.cursor_y   = i;
            editor.cursor_x   = editor_rx_to_cx(row, matched - row->render_chars);
            editor.row_offset = editor.num_rows;
            break;
        }
    }
}

void editor_op_search()
{
    const size_t saved_cx         = editor.cursor_x;
    const size_t saved_cy         = editor.cursor_y;
    const size_t saved_col_offset = editor.col_offset;
    const size_t saved_row_offset = editor.row_offset;

    char* query = editor_input_from_prompt("Search: %s (ESC to cancel)",
                                           op_search_callback);

    if (query)
    {
        free(query);
    }
    else
    {
        editor.cursor_x   = saved_cx;
        editor.cursor_y   = saved_cy;
        editor.col_offset = saved_col_offset;
        editor.row_offset = saved_row_offset;
    }
}
