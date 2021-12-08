#ifndef EDITOR_ROW_H
#define EDITOR_ROW_H

#include <stddef.h>

struct EditorRow
{
    char*  line;
    size_t length;
};

void er_write_line(struct EditorRow* er, const char* line, size_t length);

#endif  // EDITOR_ROW_H
