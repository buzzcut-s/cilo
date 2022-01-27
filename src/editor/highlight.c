#include "cilo/editor/highlight.h"

#include <ctype.h>
#include <string.h>

enum TerminalColors
{
    Red = 31,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Default = 39,
};

int eh_highlight_to_color(int highlight)
{
    switch (highlight)  // clang-format off
    {
        case HighlightDefault:  return Default;
        case HighlightNumber:   return Red;
        case HighlightKeyword2: return Green;
        case HighlightKeyword1: return Yellow;
        case HighlightMatch:    return Blue;
        case HighlightString:   return Magenta;
        case HighlightComment:  return Cyan;
        default:                return White;
    }  // clang-format on
}

bool eh_is_separator(char c)
{
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}
