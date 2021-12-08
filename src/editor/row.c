#include "cilo/editor/row.h"

#include <stdlib.h>
#include <string.h>

void er_store_line(struct EditorRow* er, const char* line, size_t length)
{
    er->length = length;
    er->line   = malloc(length + 1);
    memcpy(er->line, line, length);
    er->line[length] = '\0';
}
