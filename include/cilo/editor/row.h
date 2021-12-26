#ifndef EDITOR_ROW_H
#define EDITOR_ROW_H

#include <stddef.h>

struct EditorRow
{
    char*  line_chars;
    size_t line_length;

    char*  render_chars;
    size_t render_length;
};

void er_store_line(struct EditorRow* row, const char* line, size_t length);

void er_update_render(struct EditorRow* row);

void er_insert_character(struct EditorRow* row, size_t at, int c);

void er_delete_character(struct EditorRow* row, size_t at);

#endif  // EDITOR_ROW_H
