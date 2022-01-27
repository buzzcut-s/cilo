#ifndef EDITOR_HIGHLIGHT_H
#define EDITOR_HIGHLIGHT_H

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

int editor_highlight_to_color(int highlight);

#endif  // EDITOR_HIGHLIGHT_H
