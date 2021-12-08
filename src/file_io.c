#include "cilo/file_io.h"

#include <stdio.h>
#include <stdlib.h>

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

void read_file(const char* path)
{
    FILE* file = fopen(path, "r");
    if (!file)
        die("fopen");

    char*  line          = NULL;
    size_t line_capacity = 0;

    ssize_t length = 0;
    while ((length = getline(&line, &line_capacity, file)) != -1)
    {
        length = trim_newline(line, length);
        store_line(line, length);
    }

    free(line);
    fclose(file);
}
