#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <termios.h>

#include <cilo/editor/syntax.h>

struct EditorState
{
    struct termios original_state;

    bool is_dirty;

    uint16_t screen_rows;
    uint16_t screen_cols;

    size_t cursor_x;
    size_t cursor_y;

    size_t render_x;

    size_t row_offset;
    size_t col_offset;

    struct EditorRow* rows;

    size_t num_rows;
    size_t rows_capacity;

    const char* filename;

    struct EditorSyntax* syntax;

    time_t status_msg_time;
    char   status_msg[80];
};

extern struct EditorState editor;

void editor_state_init();

void editor_state_redraw();

void editor_state_insert_line(size_t at, const char* line, size_t length);

void editor_state_delete_line(size_t at);

void editor_state_set_status_msg(const char* format, ...);

#endif  // EDITOR_STATE_H
