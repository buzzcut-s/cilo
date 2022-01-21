#include "cilo/editor/highlight.h"

int eh_highlight_to_color(int highlight)
{
    switch (highlight)  // clang-format off
    {
        case HL_Default: return 39;
        case HL_Number: return 31;
        default: return 37;
    }  // clang-format on
}
