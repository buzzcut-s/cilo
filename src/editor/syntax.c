#include "cilo/editor/syntax.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static const char* const C_SYNTAX_MATCHES[] =
  {".c", ".h", ".cpp", NULL};

static struct EditorSyntax editor_syntax_database[] =
  {
    {"c",
     C_SYNTAX_MATCHES,
     SyntaxFlagNumbers},
};

static inline size_t syntax_db_size()
{
    return (sizeof(editor_syntax_database)
            / sizeof(editor_syntax_database[0]));
}

struct EditorSyntax* es_select_syntax_from(const char* filename)
{
    if (filename == NULL)
        return NULL;

    const char* file_ext = strrchr(filename, '.');

    for (size_t i = 0; i < syntax_db_size(); i++)
    {
        struct EditorSyntax* entry = &editor_syntax_database[i];

        size_t j = 0;
        while (entry->filetype_matches[j])
        {
            const bool match_pattern_is_ext = entry->filetype_matches[j][0] == '.';

            if ((match_pattern_is_ext && file_ext
                 && !strcmp(file_ext, entry->filetype_matches[j]))
                || (!match_pattern_is_ext
                    && strstr(filename, entry->filetype_matches[j])))
            {
                return entry;
            }

            i++;
        }
    }

    return NULL;
}
