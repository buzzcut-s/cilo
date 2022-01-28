#include "cilo/editor/row.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cilo/common.h>
#include <cilo/editor/highlight.h>
#include <cilo/editor/state.h>
#include <cilo/editor/syntax.h>
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

    row->highlights = NULL;
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

    er_update_highlight(row);
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

    row->highlights = NULL;

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
    free(row->highlights);
}

void er_update_highlight(struct EditorRow* row)
{
    if (row->render_length == 0)
        return;

    uint8_t* new_highlights = realloc(row->highlights, row->render_length);
    if (new_highlights == NULL)
        die("er_update_highlight");
    row->highlights = new_highlights;

    memset(row->highlights, HighlightNormal, row->render_length);

    if (editor.syntax == NULL)
        return;

    const char*  slcs     = editor.syntax->single_line_comment_start;
    const size_t slcs_len = slcs ? strlen(slcs) : 0;

    bool prev_was_sep   = true;
    char in_string_type = 0;

    size_t i = 0;
    while (i < row->render_length)
    {
        const char    current = row->render_chars[i];
        const uint8_t prev_hl = row->highlights[i > 0 ? i - 1 : 0];

        if (slcs_len && !in_string_type)
        {
            if (!strncmp(&row->render_chars[i], slcs, slcs_len))
            {
                memset(&row->highlights[i], HighlightComment, row->render_length - i);
                break;
            }
        }

        if (editor.syntax->flags & SyntaxFlagStrings)
        {
            if (in_string_type)
            {
                row->highlights[i] = HighlightString;

                if (current == '\\' && i + 1 < row->render_length)
                {
                    row->highlights[i + 1] = HighlightString;
                    i += 2;
                    continue;
                }

                if (current == in_string_type)
                    in_string_type = 0;

                prev_was_sep = false;
                i++;
                continue;
            }

            if (current == '"' || current == '\'')
            {
                row->highlights[i] = HighlightString;
                in_string_type     = current;
                i++;
                continue;
            }
        }

        if (editor.syntax->flags & SyntaxFlagNumbers)
        {
            if ((isdigit(current) && (prev_was_sep || prev_hl == HighlightNumber))
                || (current == '.' && prev_hl == HighlightNumber))
            {
                row->highlights[i] = HighlightNumber;
                prev_was_sep       = false;
                i++;
                continue;
            }
        }

        prev_was_sep = editor_syntax_is_separator(current);
        i++;
    }
}
