#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

enum HighlightType
{
    HighlightDefault = -1,
    HighlightNormal  = 0,
    HighlightNumber,
};

int eh_highlight_to_color(int highlight);

#endif  // HIGHLIGHT_H
