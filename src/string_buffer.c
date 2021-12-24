#include "cilo/string_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

void sbuffer_init(struct StringBuffer* sb)
{
    static const size_t BUFFER_INITIAL_CAPACITY = 256;

    sb->buffer = malloc(BUFFER_INITIAL_CAPACITY);
    if (sb->buffer == NULL)
        return;

    sb->capacity = BUFFER_INITIAL_CAPACITY;
}

void sbuffer_insert(struct StringBuffer* sb, const char* s, size_t length)
{
    if (sb->length + length > sb->capacity)
    {
        sb->capacity = (sb->capacity * 2) + length;

        char* new_buffer = realloc(sb->buffer, sb->capacity);
        if (new_buffer == NULL)
        {
            free(sb->buffer);
            sb->buffer = NULL;
            return;
        }
        sb->buffer = new_buffer;
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
