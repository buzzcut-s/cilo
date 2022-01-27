#ifndef EDITOR_ROW_H
#define EDITOR_ROW_H

#include <stddef.h>
#include <stdint.h>

struct EditorRow
{
    char*  chars;
    size_t length;

    char*  render_chars;
    size_t render_length;

    uint8_t* highlights;
};

void er_store_line(struct EditorRow* row, const char* line, size_t length);

void er_update_render(struct EditorRow* row);

void er_insert_character(struct EditorRow* row, size_t at, int c);

void er_delete_character(struct EditorRow* row, size_t at);

void er_append_string(struct EditorRow* row, const char* s, size_t length);

void er_free(struct EditorRow* row);

void er_update_highlight(struct EditorRow* row);

#endif  // EDITOR_ROW_H
