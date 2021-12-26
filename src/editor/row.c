#include "cilo/editor/row.h"

#include <stdlib.h>
#include <string.h>

#include <cilo/common.h>
#include <cilo/error.h>

void er_store_line(struct EditorRow* er, const char* line, size_t length)
{
    er->line_length = length;
    er->line_chars  = malloc(length + 1);
    memcpy(er->line_chars, line, length);
    er->line_chars[length] = '\0';
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
