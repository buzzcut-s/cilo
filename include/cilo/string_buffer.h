#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>

struct StringBuffer
{
    char*  buffer;
    size_t length;
    size_t capacity;
};

#define BUFFER_ZERO \
    {               \
        NULL, 0, 0  \
    }

void sbuffer_init(struct StringBuffer* sb);

void sbuffer_insert(struct StringBuffer* sb, const char* s, size_t length);
void sbuffer_free(struct StringBuffer* sb);

void sbuffer_flush(const struct StringBuffer* sb);

#endif  // STRING_BUFFER_H
