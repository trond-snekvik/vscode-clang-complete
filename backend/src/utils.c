#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "log.h"
#include "hashtable.h"


line_array_t * line_array(const char * p_string)
{
    if (!p_string)
    {
        return NULL;
    }

    line_array_t * p_result = MALLOC(sizeof(line_array_t));
    p_result->line_count = 1;
    for (const char * p_c = &p_string[0]; *p_c; p_c++)
    {
        if (*p_c == '\n')
        {
            p_result->line_count++;
        }
    }

    p_result->pp_lines = MALLOC(sizeof(char *) * p_result->line_count);

    const char * p_c = &p_string[0];

    for (unsigned i = 0; i < p_result->line_count; ++i)
    {
        unsigned len = strlen(p_c);
        p_result->pp_lines[i] = MALLOC(len + 1);
        memcpy(p_result->pp_lines[i], p_c, len);
        p_result->pp_lines[i][len] = '\0';
        p_c += len + 1;
    }
    return p_result;
}



void line_array_free(line_array_t * p_line_array)
{
    if (p_line_array)
    {
        for (unsigned i = 0; i < p_line_array->line_count; ++i)
        {
           FREE(p_line_array->pp_lines[i]);
        }
        FREE(p_line_array->pp_lines);
        FREE(p_line_array);
    }
}
