#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include <stddef.h>
#include <stdint.h>

#include <termios.h>

struct EditorState
{
    struct termios original_state;

    size_t cursor_x;
    size_t cursor_y;

    size_t render_x;

    size_t row_offset;
    size_t col_offset;

    uint16_t screen_rows;
    uint16_t screen_cols;

    struct EditorRow* rows;

    size_t num_rows;
    size_t rows_capacity;
};

extern struct EditorState editor;

void init_editor();

void redraw_editor();

void store_line(const char* line, size_t length);

#endif  // EDITOR_STATE_H
