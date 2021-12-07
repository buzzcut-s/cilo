#include "cilo/append_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

void buffer_insert(struct AppendBuffer* ab, const char* s, size_t length)
{
    char* result = realloc(ab->buffer, ab->length + length);
    if (result == NULL)
        return;

    memcpy(&result[ab->length], s, length);
    ab->buffer = result;
    ab->length += length;
}

void buffer_free(struct AppendBuffer* ab)
{
    free(ab->buffer);
}

void buffer_flush(const struct AppendBuffer* ab)
{
    write(STDOUT_FILENO, ab->buffer, ab->length);
}
