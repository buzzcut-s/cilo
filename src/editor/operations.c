#include "cilo/editor/operations.h"

#include <stdbool.h>

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
