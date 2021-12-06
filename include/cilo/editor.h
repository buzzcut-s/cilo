#ifndef EDITOR_H
#define EDITOR_H

#include <stdint.h>

#include <termios.h>

struct EditorState
{
    struct termios original_state;
    uint16_t       screen_rows;
    uint16_t       screen_cols;
};

extern struct EditorState editor;

void init_editor();

void process_keypress();

void clear_screen();
void refresh_screen();

#endif  // EDITOR_H
