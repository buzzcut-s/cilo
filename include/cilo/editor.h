#ifndef EDITOR_H
#define EDITOR_H

#include <stdint.h>

#include <termios.h>

struct EditorState
{
    struct termios original_state;

    int cursor_x;
    int cursor_y;

    uint16_t screen_rows;
    uint16_t screen_cols;
};

extern struct EditorState editor;

void init_editor();

void process_keypress();

void redraw_editor();

#endif  // EDITOR_H
