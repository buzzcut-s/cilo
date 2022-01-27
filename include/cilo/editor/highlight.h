#ifndef EDITOR_HIGHLIGHT_H
#define EDITOR_HIGHLIGHT_H

#include <stdbool.h>

enum EditorHighlightType
{
    HighlightDefault = -1,
    HighlightNormal  = 0,
    HighlightNumber,
    HighlightMatch,
    HighlightString,
    HighlightComment,
    HighlightKeyword1,
    HighlightKeyword2,
};

int eh_highlight_to_color(int highlight);

bool eh_is_separator(char c);

#endif  // EDITOR_HIGHLIGHT_H
