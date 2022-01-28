#ifndef EDITOR_SYNTAX_H
#define EDITOR_SYNTAX_H

#include <stdbool.h>
#include <stdint.h>

enum EditorSyntaxFlags
{
    SyntaxFlagNumbers = 1 << 0,
    SyntaxFlagStrings = 1 << 2,
};

struct EditorSyntax
{
    const char* filetype;
    const char* single_line_comment_start;

    const char* const* keywords;
    const char* const* filetype_matches;

    uint32_t flags;
};

struct EditorSyntax* editor_syntax_select_from(const char* filename);

bool editor_syntax_is_separator(char c);

#endif  // EDITOR_SYNTAX_H
