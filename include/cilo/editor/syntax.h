#ifndef EDITOR_SYNTAX_H
#define EDITOR_SYNTAX_H

#include <stdint.h>

enum EditorSyntaxFlags
{
    SyntaxFlagNumbers = 1 << 0,
    SyntaxFlagStrings = 1 << 2,
};

struct EditorSyntax
{
    const char*        filetype;
    const char* const* filetype_matches;
    const char*        single_line_comment_start;
    uint32_t           flags;
};

struct EditorSyntax* es_select_syntax_from(const char* filename);

#endif  // EDITOR_SYNTAX_H
