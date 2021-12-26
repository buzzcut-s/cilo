#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <termios.h>

struct EditorState
{
    struct termios original_state;

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

    time_t help_msg_time;
    char   help_msg[80];
};

extern struct EditorState editor;

void editor_state_init();

void editor_state_redraw();

void editor_state_store_line(const char* line, size_t length);

void editor_state_set_help_message(const char* format, ...);

#endif  // EDITOR_STATE_H
