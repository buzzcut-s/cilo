#include "cilo/editor/row.h"

#include <stdlib.h>
#include <string.h>

#include <cilo/common.h>
#include <cilo/error.h>
#include <cilo/utils.h>

void er_store_line(struct EditorRow* row, const char* line, size_t length)
{
    row->line_length = length;

    row->line_chars = malloc(length + 1);
    if (row->line_chars == NULL)
        die("er_store_line");

    memcpy(row->line_chars, line, length);
    row->line_chars[length] = '\0';
}

void er_update_render(struct EditorRow* row)
{
    size_t n_tabs = 0;

    for (size_t i = 0; i < row->line_length; i++)
    {
        if (row->line_chars[i] == '\t')
            n_tabs++;
    }

    row->render_chars = malloc(row->line_length + n_tabs * (CILO_TAB_STOP - 1) + 1);
    if (row->render_chars == NULL)
        die("update_render");

    size_t idx = 0;
    for (size_t j = 0; j < row->line_length; j++)
    {
        if (row->line_chars[j] == '\t')
        {
            row->render_chars[idx++] = ' ';
            while (idx % CILO_TAB_STOP != 0)
                row->render_chars[idx++] = ' ';
        }
        else
        {
            row->render_chars[idx++] = row->line_chars[j];
        }
    }

    row->render_length     = idx;
    row->render_chars[idx] = '\0';
}

void er_insert_character(struct EditorRow* row, size_t at, int c)
{
    at = MIN(at, row->line_length);

    char* new_line_chars = realloc(row->line_chars, row->line_length + 2);
    if (new_line_chars == NULL)
        die("er_insert_character");

    row->line_chars = new_line_chars;

    memmove(&row->line_chars[at + 1], &row->line_chars[at], row->line_length - at + 1);

    row->line_length++;
    row->line_chars[at] = c;

    er_update_render(row);
}
