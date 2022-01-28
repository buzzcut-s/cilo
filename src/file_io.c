#include "cilo/file_io.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <cilo/editor/input.h>
#include <cilo/editor/row.h>
#include <cilo/editor/state.h>
#include <cilo/editor/syntax.h>
#include <cilo/error.h>

static inline ssize_t trim_newline(const char* line, ssize_t length)
{
    while (length > 0
           && (line[length - 1] == '\n' || line[length - 1] == '\r'))
    {
        length--;
    }
    return length;
}

void file_io_open(const char* path)
{
    editor.filename = strdup(path);
    editor.syntax   = editor_syntax_select_from(editor.filename);

    FILE* file = fopen(path, "r");
    if (file == NULL)
        die("fopen");

    char*  line          = NULL;
    size_t line_capacity = 0;

    ssize_t length = 0;
    while ((length = getline(&line, &line_capacity, file)) != -1)
    {
        length = trim_newline(line, length);
        editor_state_insert_line(editor.num_rows, line, length);
    }

    free(line);
    fclose(file);

    editor.is_dirty = false;
}

void file_io_save()
{
    if (editor.filename == NULL)
    {
        editor.filename = editor_input_from_prompt("Save as: %s", NULL);
        if (editor.filename == NULL)
        {
            editor_state_set_status_msg("Save aborted.");
            return;
        }

        editor.syntax = editor_syntax_select_from(editor.filename);
        if (editor.syntax != NULL)
            editor_state_rehighlight_file();
    }

    size_t buf_len = 0;
    char*  buf     = editor_state_rows_to_string(&buf_len);

    const int fd = open(editor.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1)
    {
        if (ftruncate(fd, buf_len) != -1)
        {
            if (write(fd, buf, buf_len) == buf_len)
            {
                close(fd);
                free(buf);
                editor.is_dirty = false;
                editor_state_set_status_msg("%d bytes written to disk", buf_len);
                return;
            }
        }
        close(fd);
    }

    free(buf);
    editor_state_set_status_msg("Can't save! I/O error: %s", strerror(errno));
}
