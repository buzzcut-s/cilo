#include "cilo/string_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

static const size_t BUFFER_INITIAL_SIZE = 256;

void buffer_init(struct StringBuffer* sb)
{
    char* result = (char*)malloc(BUFFER_INITIAL_SIZE);

    /* Store NULL if allocation fails */
    sb->buffer = result;

    sb->length = 0;
}

void buffer_insert(struct StringBuffer* sb, const char* s, size_t length)
{
    if (sb->length + length > BUFFER_INITIAL_SIZE)
    {
        char* result = realloc(sb->buffer, sb->length + length);
        if (result == NULL)
            return;

        memcpy(&result[sb->length], s, length);
        sb->buffer = result;
    }
    else
    {
        if (sb->buffer == NULL)
            return;

        memcpy(&sb->buffer[sb->length], s, length);
    }

    sb->length += length;
}

void buffer_free(struct StringBuffer* sb)
{
    free(sb->buffer);
}

void buffer_flush(const struct StringBuffer* sb)
{
    write(STDOUT_FILENO, sb->buffer, sb->length);
}
