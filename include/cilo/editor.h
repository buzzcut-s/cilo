#ifndef EDITOR_H
#define EDITOR_H

#include <termios.h>

struct EditorState
{
    struct termios original_state;
};

extern struct EditorState editor;

void process_keypress();

void refresh_screen();
void clear_screen();

#endif  // EDITOR_H
