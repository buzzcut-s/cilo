#include "cilo/editor/highlight.h"

#include <ctype.h>
#include <string.h>

int eh_highlight_to_color(int highlight)
{
    switch (highlight)  // clang-format off
    {
        case HighlightDefault:  return 39;
        case HighlightNumber:   return 31;
        case HighlightKeyword2: return 32;
        case HighlightKeyword1: return 33;
        case HighlightMatch:    return 34;
        case HighlightString:   return 35;
        case HighlightComment:  return 36;
        default:                return 37;
    }  // clang-format on
}

bool eh_is_separator(char c)
{
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}
