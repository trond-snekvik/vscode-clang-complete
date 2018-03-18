#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "doxygen.h"
#include "log.h"
#include "utils.h"

#define STRLEN(STR) ((STR) ? strlen(STR) : 0)

/* Keywords can start with both @ and \ */
#define KEYWORD(STR) "@" STR, "\\" STR

static const char * mp_keywords[] =
{
    KEYWORD("param"),
    KEYWORD("retval"),
    KEYWORD("return"),
    KEYWORD("returns"),
    KEYWORD("result"),
    KEYWORD("brief"),
    KEYWORD("details")
};

typedef enum
{
    KEYWORD_PARAM,
    KEYWORD_RETVAL,
    KEYWORD_RETURN,
    KEYWORD_RETURNS,
    KEYWORD_RESULT,
    KEYWORD_BRIEF,
    KEYWORD_DETAILS,

    KEYWORD_COUNT,
} keyword_t;


typedef enum
{
    MD_STATE_NORMAL,
    MD_STATE_ITALIC,
    MD_STATE_BOLD,
    MD_STATE_CODE_INLINE,
    MD_STATE_CODE_BLOCK,
    MD_STATE_CODE_BLOCK_END,
    MD_STATE_WARNING_BLOCK,
    MD_STATE_INFO_BLOCK,
    MD_STATE_TODO_BLOCK,
    MD_STATE_PARAGRAPH,
    MD_STATE_LINK,
    MD_STATE_ESCAPED_KEYWORD,
    MD_STATE_SKIP_WORD,
    MD_STATE_SKIP_BLOCK,
} markdown_state_t;

typedef struct
{
    const char * p_keyword[2];
    markdown_state_t next_state;
} markdown_keywords_t;

#define MARKDOWN_KEYWORD(KEY, STATE) {{KEYWORD(KEY)}, STATE}
static const markdown_keywords_t m_markdown_keywords[] =
{
    MARKDOWN_KEYWORD("f$", MD_STATE_CODE_INLINE),
    MARKDOWN_KEYWORD("code", MD_STATE_CODE_BLOCK),
    MARKDOWN_KEYWORD("endcode", MD_STATE_CODE_BLOCK_END),
    MARKDOWN_KEYWORD("verbatim", MD_STATE_CODE_BLOCK),
    MARKDOWN_KEYWORD("endverbatim", MD_STATE_CODE_BLOCK_END),
    MARKDOWN_KEYWORD("f[", MD_STATE_CODE_BLOCK),
    MARKDOWN_KEYWORD("f]", MD_STATE_CODE_BLOCK_END),
    MARKDOWN_KEYWORD("note", MD_STATE_INFO_BLOCK),
    MARKDOWN_KEYWORD("remark", MD_STATE_INFO_BLOCK),
    MARKDOWN_KEYWORD("remarks", MD_STATE_INFO_BLOCK),
    MARKDOWN_KEYWORD("todo", MD_STATE_TODO_BLOCK),
    MARKDOWN_KEYWORD("warning", MD_STATE_WARNING_BLOCK),
    MARKDOWN_KEYWORD("par", MD_STATE_PARAGRAPH),
    MARKDOWN_KEYWORD("paragraph", MD_STATE_PARAGRAPH),
    MARKDOWN_KEYWORD("see", MD_STATE_LINK),
    MARKDOWN_KEYWORD("link", MD_STATE_LINK),
    MARKDOWN_KEYWORD("ref", MD_STATE_LINK),
    MARKDOWN_KEYWORD("refitem", MD_STATE_LINK),
    MARKDOWN_KEYWORD("include", MD_STATE_LINK),
    MARKDOWN_KEYWORD("includelineno", MD_STATE_LINK),
    MARKDOWN_KEYWORD("includedoc", MD_STATE_LINK),
    MARKDOWN_KEYWORD("\\", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("@", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("&", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("$", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("#", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("<", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD(">", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("%", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("\"", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD(".", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("::", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("|", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("--", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("---", MD_STATE_ESCAPED_KEYWORD),
    MARKDOWN_KEYWORD("n", MD_STATE_PARAGRAPH),
    MARKDOWN_KEYWORD("a", MD_STATE_ITALIC),
    MARKDOWN_KEYWORD("e", MD_STATE_ITALIC),
    MARKDOWN_KEYWORD("em", MD_STATE_ITALIC),
    MARKDOWN_KEYWORD("b", MD_STATE_BOLD),
    MARKDOWN_KEYWORD("c", MD_STATE_CODE_INLINE),
    MARKDOWN_KEYWORD("p", MD_STATE_CODE_INLINE),
};
#undef MARKDOWN_KEYWORD
#undef KEYWORD


static bool is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static const char * skip_whitespace(const char * p_start)
{
    while (is_whitespace(*p_start))
        p_start++;
    return p_start;
}

static const char * next_whitespace(const char * p_start)
{
    while (!is_whitespace(*p_start) && *p_start != '\0')
        p_start++;
    return p_start;
}

static char * remove_duplicate_whitespace(char * p_string)
{
    char * p_src = p_string;
    char * p_dst = p_string;

    char * p_last_non_whitespace = &p_string[STRLEN(p_string) - 1];
    while (p_last_non_whitespace != p_string && is_whitespace(*p_last_non_whitespace))
    {
        p_last_non_whitespace--;
    }

    unsigned space = 1;
    unsigned newline = 2;
    while (*p_src && p_src <= p_last_non_whitespace)
    {
        if (*p_src == ' ')
            space++;
        else
            space = 0;
        if (*p_src == '\n')
            newline++;
        else
            newline = 0;

        if (space <= 1 && newline <= 2)
            *(p_dst++) = *p_src;
        p_src++;
    }
    *p_dst = '\0';
    return p_string;
}

static char * trim_whitespace(char * p_string)
{
    char * p_src = p_string;
    char * p_dst = p_string;
    char * p_last_non_whitespace = &p_string[STRLEN(p_string) - 1];
    while (p_last_non_whitespace != p_string && is_whitespace(*p_last_non_whitespace))
    {
        p_last_non_whitespace--;
    }
    while (is_whitespace(*p_src))
    {
        p_src++;
    }
    while (p_src <= p_last_non_whitespace)
    {
        *(p_dst++) = *(p_src++);
    }
    *p_dst = '\0';
    return p_string;
}

static const char * next_word(const char * p_start)
{
    while ((*p_start >= 'a' && *p_start <= 'z') ||
           (*p_start >= 'A' && *p_start <= 'Z') ||
           (*p_start >= '0' && *p_start <= '9') ||
           *p_start == '_')
        p_start++;
    return skip_whitespace(p_start);
}

static const char * next_block(const char * p_start)
{
    if (p_start)
    {
        const char * p_end = strstr(p_start, "\n\n");
        if (p_end)
            p_end += 2; // skip the line endings
        else
            p_end = p_start + STRLEN(p_start);
        return p_end;
    }
    return NULL;
}

static unsigned copy_word(char ** pp_dst, const char ** pp_src)
{
    const char * p_end = next_word(*pp_src);
    unsigned len = (p_end - *pp_src);
    memcpy(*pp_dst, *pp_src, len);
    *pp_src += len;
    *pp_dst += len;
    return len;
}

static unsigned copy_block(char ** pp_dst, const char ** pp_src)
{
    const char * p_end = next_block(*pp_src);
    unsigned len = (p_end - *pp_src);
    memcpy(*pp_dst, *pp_src, len);
    *pp_src += len;
    *pp_dst += len;
    return len;
}

static unsigned copy_string(char ** pp_dst, const char * p_src)
{
    ASSERT(p_src);
    ASSERT(pp_dst);
    ASSERT(*pp_dst);
    size_t len = strlen(p_src);
    memcpy(*pp_dst, p_src, len);
    *pp_dst += len;
    return len;
}

static bool starts_with(const char * p_string, const char * p_substr)
{
    ASSERT(p_string);
    ASSERT(p_substr);
    return (memcmp(p_string, p_substr, strlen(p_substr)) == 0);
}

static bool is_doxyblock(const char * p_string)
{
    return (starts_with(p_string, "/**") ||
            starts_with(p_string, "/*!") ||
            starts_with(p_string, "///") ||
            starts_with(p_string, "//1"));
}

static unsigned block_length(const char * p_string)
{
    const char * p_c;
    for (p_c = &p_string[0]; *p_c != '\0'; p_c++)
    {
        if (*p_c == '@' || *p_c == '\\')
        {
            for (unsigned i = 0; i < ARRAY_SIZE(mp_keywords); ++i)
            {
                if (starts_with(p_c, mp_keywords[i]))
                {
                    return (p_c - p_string);
                }
            }
        }
    }
    return (p_c - p_string);
}

static const char * get_block(keyword_t keyword, const char * p_string)
{
    for (unsigned i = 0; i < 2; ++i)
    {
        const char * p_keyword = mp_keywords[keyword * 2 + i];
        ASSERT(p_keyword);
        const char * p_start = strstr(p_string, p_keyword);
        if (p_start)
        {
            p_start += strlen(p_keyword);
            return skip_whitespace(p_start);
        }
    }

    return NULL;
}

static const char * get_first_block(const keyword_t * p_keywords, unsigned keyword_count, const char * p_string, unsigned * p_index)
{
    const char * p_first = NULL;
    for (unsigned i = 0; i < keyword_count; ++i)
    {
        const char * p_block = get_block(p_keywords[i], p_string);
        if (p_block && (!p_first || p_first > p_block))
        {
            p_first = p_block;
            *p_index = i;
        }
    }
    return p_first;
}

static char * format(const char * p_string, unsigned maxlen)
{
    ASSERT(p_string);
    unsigned string_len = strlen(p_string);
    char * p_retval = MALLOC(min(string_len, maxlen));
    char * p_dst = &p_retval[0];
    const char * p_src = &p_string[0];

    bool newline = true;
    bool margin = true;

    /* Skip any space, then grouping of /, *, < or ! at the beginning of a line, then at most 1 space. */
    for (unsigned i = 0; i < maxlen && *p_src != '\0'; ++i)
    {
        switch (*p_src)
        {
            case '\n':
                *(p_dst++) = *p_src;
                newline = true;
                margin = false;
                break;
            case ' ':
                if (!newline)
                    *(p_dst++) = *p_src;
                if (margin)
                {
                    newline = false;
                    margin = false;
                }
                break;
            case '*':
            case '/':
            case '!':
            case '<':
                if (newline)
                    margin = true;
                else
                    *(p_dst++) = *p_src;
                break;
            case '\r':
                break;
            default:
                *(p_dst++) = *p_src;
                newline = false;
                margin = false;
                break;
        }
        p_src++;
    }
    *p_dst = '\0';
    return p_retval;
}

/**
 * Parse an argument, beginning on the first character after the @param or @arg keyword.
 * Expects one of the following formats:
 *
 * @param[in,out] param Description
 * @param param Description
 * @param p1,p2,p3 Description
 */
static doxygen_arg_t * doxygen_arg_parse(const char ** pp_comment)
{
    const char * p_start = *pp_comment;
    doxygen_arg_t * p_arg = CALLOC(1, sizeof(doxygen_arg_t));
    if (*pp_comment[0] == '[')
    {
        const char * p_dir_end = strchr(*pp_comment, ']');
        const char * p_in = strstr(*pp_comment, "in");
        const char * p_out = strstr(*pp_comment, "out");
        if (p_in && p_in < p_dir_end)
        {
            p_arg->dir |= DOXYGEN_ARG_DIRECTION_IN;
        }
        if (p_out && p_out < p_dir_end)
        {
            p_arg->dir |= DOXYGEN_ARG_DIRECTION_OUT;
        }
        *pp_comment = p_dir_end + 1;
    }

    *pp_comment = skip_whitespace(*pp_comment);

    if (*pp_comment[0] == '@' || *pp_comment[0] == '\\' || *pp_comment[0] == '\0')
    {
        FREE((void *) p_arg->description);
        FREE((void *) p_arg->p_name);
        FREE(p_arg);
        *pp_comment = p_start;
        return NULL;
    }

    const char * p_name_end;
    do
    {
        p_name_end = next_word(*pp_comment);
    } while (*p_name_end == ',');

    char * p_name = CALLOC(p_name_end - *pp_comment + 1, 1);
    memcpy(p_name, *pp_comment, p_name_end - *pp_comment);
    p_arg->p_name = trim_whitespace(p_name);
    *pp_comment = skip_whitespace(p_name_end);

    const char * p_description_end = *pp_comment + block_length(*pp_comment);
    if (p_description_end != *pp_comment)
    {
        char * p_description = CALLOC(p_description_end - *pp_comment + 1, 1);
        memcpy(p_description, *pp_comment, p_description_end - *pp_comment);
        p_arg->description = trim_whitespace(p_description);
        *pp_comment = p_description_end;
    }

    return p_arg;
}

static doxygen_return_t * doxygen_return_parse(const char ** pp_comment, bool is_retval)
{
    doxygen_return_t * p_return = MALLOC(sizeof(doxygen_return_t));
    if (is_retval)
    {
        *pp_comment = skip_whitespace(*pp_comment);
        const char * p_end = next_whitespace(*pp_comment);
        char * p_value = CALLOC(p_end - *pp_comment + 1, 1);
        memcpy(p_value, *pp_comment, p_end - *pp_comment);
        p_return->p_value = trim_whitespace(p_value);
        *pp_comment = skip_whitespace(p_end);
    }
    else
    {
        p_return->p_value = NULL;
    }

    size_t description_len = block_length(*pp_comment);
    /* The description will automatically end at the next \n\n */
    const char * p_next_block = next_block(*pp_comment);
    if (p_next_block)
    {
        description_len = min(description_len, (size_t) (p_next_block - *pp_comment));
    }

    if (description_len > 0)
    {
        char * p_description = CALLOC(description_len + 1, 1);
        memcpy(p_description, *pp_comment, description_len);
        p_return->description = p_description;
    }
    else
    {
        p_return->description = NULL;
    }
    return p_return;
}

doxygen_function_t * doxygen_function_parse(const char * p_comment)
{
    ASSERT(p_comment);
    doxygen_function_t * p_function = CALLOC(1, sizeof(doxygen_function_t));

    char * p_formatted = format(p_comment, strlen(p_comment));

    const char * p_brief = get_block(KEYWORD_BRIEF, p_formatted);
    if (!p_brief)
        p_brief = p_formatted;

    unsigned brief_len = block_length(p_brief);
    /* The brief will automatically end at the next \n\n */
    const char * p_next_block = next_block(p_brief);
    if (p_next_block)
    {
        brief_len = min(brief_len, (unsigned) (p_next_block - p_brief));
    }

    if (brief_len > 0)
    {
        char * p_brief_buf = CALLOC(brief_len + 1, sizeof(char));
        memcpy(p_brief_buf, p_brief, brief_len);
        p_function->brief = p_brief_buf;
    }

    const char * p_details = get_block(KEYWORD_DETAILS, p_formatted);
    if (!p_details)
        p_details = &p_brief[brief_len];

    unsigned details_len = block_length(p_details);
    if (details_len > 0)
    {
        char * p_details_buf = CALLOC(details_len + 1, sizeof(char));
        memcpy(p_details_buf, p_details, details_len);
        p_function->details = p_details_buf;
    }

    /* go through all parameters */
    const char * p_arg_doc = get_block(KEYWORD_PARAM, p_formatted);
    while (p_arg_doc)
    {
        doxygen_arg_t * p_arg = doxygen_arg_parse(&p_arg_doc);
        if (p_arg)
        {
            p_function->p_args = REALLOC(p_function->p_args, sizeof(doxygen_arg_t) * (p_function->arg_count + 1));
            p_function->p_args[p_function->arg_count] = *p_arg;
            FREE(p_arg);
            p_function->arg_count++;
            p_arg_doc = get_block(KEYWORD_PARAM, p_arg_doc);
        }
        else
        {
            break;
        }
    }

    /* Parse returns. Can be one of "return", "returns", "retval" and "result" */
    static const keyword_t p_keywords[] =
    {
        KEYWORD_RESULT,
        KEYWORD_RETURN,
        KEYWORD_RETURNS,
        KEYWORD_RETVAL,
    };
    unsigned index;
    const char * p_return_doc = get_first_block(p_keywords, ARRAY_SIZE(p_keywords), p_formatted, &index);
    while (p_return_doc)
    {
        doxygen_return_t * p_return = doxygen_return_parse(&p_return_doc, (p_keywords[index] == KEYWORD_RETVAL));
        if (p_return)
        {
            p_function->p_returns = REALLOC(p_function->p_returns, sizeof(doxygen_return_t) * (p_function->return_count + 1));
            p_function->p_returns[p_function->return_count] = *p_return;
            FREE(p_return);
            p_function->return_count++;
            p_return_doc = get_first_block(p_keywords, ARRAY_SIZE(p_keywords), p_return_doc, &index);
        }
        else
        {
            break;
        }

    }
    return p_function;
}


char * doxygen_to_markdown(const char * string, bool skip_newline)
{
    if (!string)
    {
        return NULL;
    }
    const unsigned max_increase = 2;
    size_t maxlen = max_increase * strlen(string);
    char * p_markdown = CALLOC(maxlen + 1, 1);
    char * p_dst = p_markdown;
    const char * p_src = string;

    markdown_state_t state = MD_STATE_NORMAL;

    while (*p_src != '\0')
    {
        if (*p_src == '@' || *p_src == '\\')
        {
            bool found_keyword = false;
            markdown_state_t next_state;
            const char * p_keyword = NULL;
            for (unsigned i = 0; i < ARRAY_SIZE(m_markdown_keywords); ++i)
            {
                for (unsigned j = 0; j < ARRAY_SIZE(m_markdown_keywords[i].p_keyword); ++j)
                {
                    if (starts_with(p_src, m_markdown_keywords[i].p_keyword[j]))
                    {
                        found_keyword = true;
                        p_keyword = m_markdown_keywords[i].p_keyword[j] + 1;
                        next_state = m_markdown_keywords[i].next_state;
                        p_src = skip_whitespace(p_src + strlen(m_markdown_keywords[i].p_keyword[j]));
                        break;
                    }
                }
                if (found_keyword)
                {
                    break;
                }
            }

            if (found_keyword)
            {
                if (state == MD_STATE_NORMAL)
                {
                    switch (next_state)
                    {
                        case MD_STATE_BOLD:
                            copy_string(&p_dst, "**");
                            copy_word(&p_dst, &p_src);
                            copy_string(&p_dst, "**");
                            break;
                        case MD_STATE_ITALIC:
                            copy_string(&p_dst, "_");
                            copy_word(&p_dst, &p_src);
                            copy_string(&p_dst, "_");
                            break;
                        case MD_STATE_CODE_INLINE:
                            copy_string(&p_dst, "`");
                            copy_word(&p_dst, &p_src);
                            copy_string(&p_dst, "`");
                            break;
                        case MD_STATE_CODE_BLOCK:
                            copy_string(&p_dst, "```");
                            /* Parse language specifier: */
                            if (p_src[0] == '{')
                            {
                                const char * p_lang_end = strchr(p_src, '}');
                                if (p_lang_end && p_lang_end < &p_src[20]) // assuming no language has a longer name than 20 characters
                                {
                                    unsigned len = p_lang_end - &p_src[1];
                                    memcpy(p_dst, &p_src[1], len);
                                    p_dst += len;
                                    p_src += len + 2;
                                }
                            }
                            copy_string(&p_dst, "\n");
                            state = next_state;
                            break;
                        case MD_STATE_PARAGRAPH:
                            copy_string(&p_dst, "\n\n");
                            break;
                        case MD_STATE_WARNING_BLOCK:
                        {
                            const char * header = "\n\n> **WARNING:** ";
                            copy_string(&p_dst, header);
                            copy_block(&p_dst, &p_src);
                            copy_string(&p_dst, "\n\n");
                            break;
                        }
                        case MD_STATE_INFO_BLOCK:
                        {
                            const char * header = "\n\n> **INFO:** ";
                            copy_string(&p_dst, header);
                            copy_block(&p_dst, &p_src);
                            copy_string(&p_dst, "\n\n");
                            break;
                        }
                        case MD_STATE_TODO_BLOCK:
                        {
                            const char * header = "\n\n> **TODO:** ";
                            copy_string(&p_dst, header);
                            copy_block(&p_dst, &p_src);
                            copy_string(&p_dst, "\n\n");
                            break;
                        }
                        case MD_STATE_ESCAPED_KEYWORD:
                            copy_string(&p_dst, p_keyword);
                            break;
                        case MD_STATE_SKIP_BLOCK:
                            p_src = next_block(p_src);
                            break;
                        case MD_STATE_SKIP_WORD:
                            p_src = next_word(p_src);
                            break;
                        case MD_STATE_LINK:
                            copy_word(&p_dst, &p_src);
                            break;
                    }
                }
                else
                {
                    switch (state)
                    {
                        case MD_STATE_CODE_BLOCK:
                            if (next_state == MD_STATE_CODE_BLOCK_END)
                            {
                                copy_string(&p_dst, "\n```\n\n");
                                state = MD_STATE_NORMAL;
                            }
                            else
                            {
                                copy_string(&p_dst, p_keyword);
                            }
                            break;
                    }
                }
            }
            else
            {
                /* Skip unknown keywords, and log them */
                const char * p_end = next_word(p_src);

                char * p_keyword = CALLOC(p_end - p_src + 1, 1);
                memcpy(p_keyword, p_src, p_end - p_src);
                LOG("UNKNOWN KEYWORD: %s\n", p_keyword);
                FREE(p_keyword);

                p_src = p_end + 1;
            }
        }
        else
        {
            if (skip_newline && *p_src == '\n')
                *(p_dst++) = ' ';
            else
                *(p_dst++) = *p_src;
            p_src++;
        }
        ASSERT(p_dst <= p_markdown + maxlen);
    }
    *p_dst = '\0';

    return p_markdown;
}

char * doxygen_function_to_markdown(const doxygen_function_t * p_func, bool include_params, bool include_returns)
{
    if (p_func == NULL)
    {
        return NULL;
    }

    static const char * p_directions[] =
    {
        "",
        "`in`",
        "`out`",
        "`in, out`"
    };
    static const char * p_section_header = "\n\n---\n\n";
    static const char * p_param_header_direction = "| Direction | Parameter | Description\n|-------------|-----------|--------------\n";
    static const char * p_param_header = "| Parameter | Description\n|-----------|--------------\n";
    static const char * p_return_header_with_val = "| Return value | Description\n|---------------------|--------------\n";
    static const char * p_return_header = "| Return description\n|--------------------------------------\n";

    unsigned len = strlen(p_section_header) +
                   strlen(p_param_header_direction) +
                   STRLEN(p_func->brief) +
                   STRLEN(p_func->details) +
                   strlen(p_return_header_with_val);

    bool has_param_directions = false;
    if (include_params)
    {
        const unsigned per_param_overhead = 20;
        for (unsigned i = 0; i < p_func->arg_count; ++i)
        {
            len += per_param_overhead + STRLEN(p_func->p_args[i].p_name) + 2 * STRLEN(p_func->p_args[i].description) + STRLEN(p_directions[DOXYGEN_ARG_DIRECTION_IN_OUT]);

            if (p_func->p_args[i].dir != DOXYGEN_ARG_DIRECTION_NONE)
            {
                has_param_directions = true;
            }
        }
    }

    bool has_retvals = false;
    if (include_returns)
    {
        const unsigned per_return_overhead = 20;
        for (unsigned i = 0; i < p_func->return_count; ++i)
        {
            len += per_return_overhead + strlen(p_func->p_returns[i].p_value) + 2 * strlen(p_func->p_returns[i].description);
            if (p_func->p_returns[i].p_value)
            {
                has_retvals = true;
            }
        }
    }

    char * p_output = MALLOC(len);
    char * p_dst = p_output;
    p_dst += sprintf(p_dst, "%s%s\n", p_section_header, doxygen_to_markdown(p_func->brief, false));

    if (p_func->details)
    {
        p_dst += sprintf(p_dst, "%s", doxygen_to_markdown(p_func->details, false));
    }

    if (p_func->p_args && include_params)
    {
        p_dst += sprintf(p_dst, "%s", has_param_directions ? p_param_header_direction : p_param_header);

        for (unsigned i = 0; i < p_func->arg_count; ++i)
        {
            char * p_desc = doxygen_to_markdown(p_func->p_args[i].description, true);
            if (has_param_directions)
            {
                p_dst += sprintf(p_dst, "| %s ", p_directions[p_func->p_args[i].dir]);
            }
            p_dst += sprintf(p_dst, "| `%s` | %s \n", p_func->p_args[i].p_name, p_desc ? p_desc : "");
            FREE(p_desc);
        }
    }

    if (p_func->p_returns && include_returns)
    {
        p_dst += sprintf(p_dst, "\n%s", has_retvals ? p_return_header_with_val : p_return_header);

        for (unsigned i = 0; i < p_func->return_count; ++i)
        {
            char * p_desc = doxygen_to_markdown(p_func->p_returns[i].description, true);
            if (p_func->p_returns[i].p_value)
                p_dst += sprintf(p_dst, "| `%s` ", p_func->p_returns[i].p_value);
            p_dst += sprintf(p_dst, "| %s\n", p_desc ? p_desc : "");
            FREE(p_desc);
        }
    }

    *p_dst = '\0';
    return p_output;
}

void doxygen_function_free(doxygen_function_t * p_function)
{
    if (p_function)
    {
        FREE(p_function->brief);
        FREE(p_function->details);
        for (unsigned i = 0; i < p_function->arg_count; ++i)
        {
            FREE(p_function->p_args[i].description);
            FREE(p_function->p_args[i].p_name);
        }
        for (unsigned i = 0; i < p_function->return_count; ++i)
        {
            FREE(p_function->p_returns[i].description);
            FREE(p_function->p_returns[i].p_value);
        }
        FREE(p_function);
    }
}

