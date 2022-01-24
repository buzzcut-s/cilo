#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <stdbool.h>

enum HighlightType
{
    HighlightDefault = -1,
    HighlightNormal  = 0,
    HighlightNumber,
    HighlightMatch,
};

int eh_highlight_to_color(int highlight);

bool eh_is_separator(char c);

#endif  // HIGHLIGHT_H
