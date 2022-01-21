#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

enum HighlightType
{
    HL_Default = -1,  // NOLINT
    HL_Number  = 0,   // NOLINT
    HL_Normal,        // NOLINT
};

int eh_highlight_to_color(int highlight);

#endif  // HIGHLIGHT_H
