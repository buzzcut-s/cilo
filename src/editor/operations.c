#include "cilo/editor/operations.h"

#include <stdbool.h>

#include <cilo/editor/row.h>
#include <cilo/editor/state.h>

void editor_op_insert_char(int c)
{
    if (editor.cursor_y == editor.num_rows)
        editor_state_store_line("", 0);

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
        editor.cursor_x = editor.rows[editor.cursor_y - 1].line_length;
        er_append_string(&editor.rows[editor.cursor_y - 1], row->line_chars, row->line_length);
        editor_state_delete_line(editor.cursor_y);
        editor.cursor_y--;
    }

    editor.is_dirty = true;
}
