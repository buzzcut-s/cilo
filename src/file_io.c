#include "cilo/file_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cilo/editor/state.h>
#include <cilo/error.h>

static ssize_t trim_newline(const char* line, ssize_t length)
{
    while (length > 0
           && (line[length - 1] == '\n' || line[length - 1] == '\r'))
    {
        length--;
    }
    return length;
}

void file_io_read(const char* path)
{
    editor.filename = strdup(path);

    FILE* file = fopen(path, "r");
    if (!file)
        die("fopen");

    char*  line          = NULL;
    size_t line_capacity = 0;

    ssize_t length = 0;
    while ((length = getline(&line, &line_capacity, file)) != -1)
    {
        length = trim_newline(line, length);
        editor_state_store_line(line, length);
    }

    free(line);
    fclose(file);
}
