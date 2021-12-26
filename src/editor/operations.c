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
