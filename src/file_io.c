#include "cilo/file_io.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <cilo/editor/row.h>
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

    editor.is_dirty = false;
}

static char* er_to_string(size_t* out_buf_len)
{
    size_t total_len = 0;

    for (size_t i = 0; i < editor.num_rows; i++)
        total_len += editor.rows[i].line_length + 1;
    *out_buf_len = total_len;

    char* buf = malloc(total_len);
    if (buf == NULL)
        die("er_to_string");

    char* p = buf;
    for (size_t i = 0; i < editor.num_rows; i++)
    {
        memcpy(p, editor.rows[i].line_chars, editor.rows[i].line_length);
        p += editor.rows[i].line_length;
        *p = '\n';
        p++;
    }

    return buf;
}

void file_io_save()
{
    if (editor.filename == NULL)
        return;

    size_t buf_len = 0;
    char*  buf     = er_to_string(&buf_len);

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
