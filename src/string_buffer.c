#include "cilo/string_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

void sbuffer_init(struct StringBuffer* sb)
{
    static const size_t BUFFER_INITIAL_SIZE = 256;

    sb->buffer = malloc(BUFFER_INITIAL_SIZE);
    if (sb->buffer == NULL)
        return;

    sb->capacity = BUFFER_INITIAL_SIZE;
}

void sbuffer_insert(struct StringBuffer* sb, const char* s, size_t length)
{
    if (sb->length + length > sb->capacity)
    {
        sb->capacity = (sb->capacity * 2) + length;
        sb->buffer   = realloc(sb->buffer, sb->capacity);
        if (sb->buffer == NULL)
            return;
    }

    memcpy(&sb->buffer[sb->length], s, length);

    sb->length += length;
}

void sbuffer_free(struct StringBuffer* sb)
{
    free(sb->buffer);
}

void sbuffer_flush(const struct StringBuffer* sb)
{
    write(STDOUT_FILENO, sb->buffer, sb->length);
}
