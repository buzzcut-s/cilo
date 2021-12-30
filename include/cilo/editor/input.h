#ifndef EDITOR_INPUT_H
#define EDITOR_INPUT_H

void editor_input_process();

char* editor_input_from_prompt(const char* prompt,
                               void (*search_callback)(const char*, int));

#endif  // EDITOR_INPUT_H
