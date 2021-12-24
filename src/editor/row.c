#include "cilo/editor/row.h"

#include <stdlib.h>
#include <string.h>

void er_store_line(struct EditorRow* er, const char* line, size_t length)
{
    er->line_length = length;
    er->line_chars  = malloc(length + 1);
    memcpy(er->line_chars, line, length);
    er->line_chars[length] = '\0';
}
