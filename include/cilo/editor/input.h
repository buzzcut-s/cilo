#ifndef EDITOR_INPUT_H
#define EDITOR_INPUT_H

enum EditorInputKey
{
    KeyBackspace = 127,
    KeyArrowUp   = 2103,
    KeyArrowLeft,
    KeyArrowDown,
    KeyArrowRight,
    KeyPageUp,
    KeyPageDown,
    KeyHome,
    KeyEnd,
    KeyDelete,
};

void editor_input_process();

char* editor_input_from_prompt(const char* prompt,
                               void (*search_callback)(const char*, int));

#endif  // EDITOR_INPUT_H
