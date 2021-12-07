#ifndef APPEND_BUFFER_H
#define APPEND_BUFFER_H

#include <stddef.h>

struct AppendBuffer
{
    char*  buffer;
    size_t length;
};

void buffer_init(struct AppendBuffer* ab);

void buffer_insert(struct AppendBuffer* ab, const char* s, size_t length);
void buffer_free(struct AppendBuffer* ab);

void buffer_flush(const struct AppendBuffer* ab);

#endif  // APPEND_BUFFER_H
