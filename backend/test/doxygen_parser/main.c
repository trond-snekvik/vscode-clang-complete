#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "doxygen.h"
#include "log.h"

const char * m_input =
"  /**\n"
"   * \\brief The kind of entity that this completion refers to.\n"
"   *\n"
"   * The cursor kind will be a macro, keyword, or a declaration (one of the\n"
"   * *Decl cursor kinds), describing the entity that the completion is\n"
"   * referring to.\n"
"   *\n"
"   * \\todo In the future, we would like to provide a full cursor, to allow\n"
"   * the client to extract additional information from declaration.\n"
"   */\n"
;
char g_log_buf[LOG_BUFFER_SIZE];

static const char * mp_directions[] =
{
    "",
    "`in`",
    "`out`",
    "`in, out`"
};

void assert_handler(const char * p_file, unsigned line)
{
    printf("ASSERT @ %s:%u\n", p_file, line);
    fflush(stdout);
    exit(1);
}

int main()
{
    const doxygen_function_t * p_func = doxygen_function_parse(m_input);
    if (p_func)
    {
        const char * p_out = doxygen_function_to_markdown(p_func);
        if (p_out)
        {
            printf("%s\n", p_out);
            free(p_out);
        }
        printf("\n---\n");
        free(p_func);
    }
    else
    {
        printf("# No function\n");
    }
}