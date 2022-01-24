#include "cilo/editor/highlight.h"

int eh_highlight_to_color(int highlight)
{
    switch (highlight)  // clang-format off
    {
        case HighlightDefault: return 39;
        case HighlightNumber:  return 31;
        case HighlightMatch:   return 34;
        default:               return 37;
    }  // clang-format on
}
