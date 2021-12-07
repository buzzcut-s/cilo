#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>

struct StringBuffer
{
    char*  buffer;
    size_t length;
};

void buffer_init(struct StringBuffer* sb);

void buffer_insert(struct StringBuffer* sb, const char* s, size_t length);
void buffer_free(struct StringBuffer* sb);

void buffer_flush(const struct StringBuffer* sb);

#endif  // STRING_BUFFER_H
