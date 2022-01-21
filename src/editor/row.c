#include "cilo/editor/row.h"

#include <stdlib.h>
#include <string.h>

#include <cilo/common.h>
#include <cilo/error.h>
#include <cilo/utils.h>

void er_store_line(struct EditorRow* row, const char* line, size_t length)
{
    row->length = length;

    row->chars = malloc(length + 1);
    if (row->chars == NULL)
        die("er_store_line");

    memcpy(row->chars, line, length);
    row->chars[length] = '\0';
}

void er_update_render(struct EditorRow* row)
{
    size_t n_tabs = 0;
    for (size_t i = 0; i < row->length; i++)
    {
        if (row->chars[i] == '\t')
            n_tabs++;
    }

    row->render_chars = malloc(row->length + n_tabs * (CILO_TAB_STOP - 1) + 1);
    if (row->render_chars == NULL)
        die("er_update_render");

    size_t idx = 0;
    for (size_t i = 0; i < row->length; i++)
    {
        if (row->chars[i] == '\t')
        {
            row->render_chars[idx++] = ' ';
            while (idx % CILO_TAB_STOP != 0)
            {
                row->render_chars[idx++] = ' ';
            }
        }
        else
        {
            row->render_chars[idx++] = row->chars[i];
        }
    }

    row->render_length     = idx;
    row->render_chars[idx] = '\0';
}

void er_insert_character(struct EditorRow* row, size_t at, int c)
{
    char* new_chars = realloc(row->chars, row->length + 2);
    if (new_chars == NULL)
        die("er_insert_character");

    row->chars = new_chars;

    memmove(&row->chars[at + 1], &row->chars[at], row->length - at + 1);

    row->length++;
    row->chars[at] = c;

    er_update_render(row);
}

void er_delete_character(struct EditorRow* row, size_t at)
{
    memmove(&row->chars[at], &row->chars[at + 1], row->length - at);

    row->length--;

    er_update_render(row);
}

void er_append_string(struct EditorRow* row, const char* s, size_t length)
{
    char* new_chars = realloc(row->chars, row->length + length + 1);
    if (new_chars == NULL)
        die("er_append_string");

    row->chars = new_chars;

    memcpy(&row->chars[row->length], s, length);
    row->length += length;

    er_update_render(row);
}

void er_free(struct EditorRow* row)
{
    free(row->chars);
    free(row->render_chars);
}
