#include "cilo/editor/syntax.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static const char* const C_SYNTAX_MATCHES[] =
  {".c", ".h", ".cpp", ".hpp", NULL};

static const char* const C_SYNTAX_KEYWORDS[] =
  {"switch", "if", "while", "for", "break", "continue", "return", "else",
   "struct", "union", "typedef", "static", "enum", "class", "case",

   "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
   "void|", NULL};

static struct EditorSyntax editor_syntax_database[] =
  {
    {.filetype                  = "c",
     .filetype_matches          = C_SYNTAX_MATCHES,
     .keywords                  = C_SYNTAX_KEYWORDS,
     .single_line_comment_start = "//",
     .flags                     = SyntaxFlagNumbers | SyntaxFlagStrings},
};

static inline size_t syntax_db_size()
{
    return (sizeof(editor_syntax_database)
            / sizeof(editor_syntax_database[0]));
}

struct EditorSyntax* editor_syntax_select_from(const char* filename)
{
    if (filename == NULL)
        return NULL;

    const char* file_ext = strrchr(filename, '.');

    for (size_t i = 0; i < syntax_db_size(); i++)
    {
        struct EditorSyntax* entry = &editor_syntax_database[i];

        size_t match_idx = 0;
        while (entry->filetype_matches[match_idx])
        {
            const bool match_pattern_is_ext = entry->filetype_matches[match_idx][0] == '.';

            if ((match_pattern_is_ext && file_ext
                 && !strcmp(file_ext, entry->filetype_matches[match_idx]))
                || (!match_pattern_is_ext
                    && strstr(filename, entry->filetype_matches[match_idx])))
            {
                return entry;
            }

            match_idx++;
        }
    }

    return NULL;
}

bool editor_syntax_is_separator(char c)
{
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}
