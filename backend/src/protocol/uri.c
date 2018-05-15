#include "uri.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "path.h"
#include "log.h"

#define URI_SCHEME_END "://"
#define URI_AUTHORITY_END "/"
#define URI_PATH_END "?"
#define URI_QUERY_END "#"
#define URI_FRAGMENT_END "\0"

#define URI_RESERVED_CHARACTERS ":?#[]@"
#define SERIALIZED_URI_ELEM_COUNT 5
#define MANDATORY_ELEM_COUNT 2

typedef struct
{
    char ** pp_element;
    const char * p_termination_string;
} serialized_uri_t[SERIALIZED_URI_ELEM_COUNT];

#define URI_SERIALIZED(p_URI) { \
        {(char **) &(p_URI)->scheme, URI_SCHEME_END}, \
        {(char **) &(p_URI)->authority, URI_AUTHORITY_END}, \
        {(char **) &(p_URI)->path, URI_PATH_END}, \
        {(char **) &(p_URI)->query, URI_QUERY_END}, \
        {(char **) &(p_URI)->fragment, URI_FRAGMENT_END}, \
    }



static void decode_escape(char * p_out, const char * p_in, uint32_t max_count)
{
    uint8_t value;

    while (*p_in && max_count--)
    {
        if (*p_in == '%')
        {
            value = 0;
            p_in++;
            for (uint32_t i = 0; i < 2; ++i)
            {
                value *= 16;
                if (*p_in >= '0' && *p_in <= '9')
                {
                    value += *p_in - '0';
                }
                else if (*p_in >= 'a' && *p_in <= 'f')
                {
                    value += *p_in - 'a' + 10;
                }
                else if (*p_in >= 'A' && *p_in <= 'F')
                {
                    value += *p_in - 'A' + 10;
                }
                else
                {
                    *p_out = '\0';
                    return;
                }
                p_in++;
            }
            *(p_out++) = value;
        }
        else
        {
            *(p_out++) = *(p_in++);
        }
    }
    *p_out = '\0';
}

static size_t strcpy_encoded(char * p_dst, const char * p_src)
{
    char * p_c = p_dst;
    while (*p_src)
    {
        if (strchr(URI_RESERVED_CHARACTERS, *p_src))
        {
            char c[2] = {*p_src >> 4, *p_src & 0x0F};
            static const char hex[16] = "0123456789abcdef";
            *(p_c++) = '%';
            *(p_c++) = hex[c[0]];
            *(p_c++) = hex[c[1]];
        }
        else
        {
            *(p_c++) = *p_src;
        }
        p_src++;
    }
    *p_c = '\0';
    return p_c - p_dst;
}

static char * strdup_until(const char * p_str, const char * p_end)
{
    ASSERT(p_end - p_str < 1024);
    char * p_retval = CALLOC(1, (p_end - p_str) + 1);
    decode_escape(p_retval, p_str, p_end - p_str);
    return p_retval;
}

uri_t uri_decode(const char * p_raw)
{
    const char * p_end = p_raw + strlen(p_raw);
    uri_t uri;
    memset(&uri, 0, sizeof(uri));

    const serialized_uri_t serialized = URI_SERIALIZED(&uri);

    for (uint32_t i = 0; i < SERIALIZED_URI_ELEM_COUNT; ++i)
    {
        const char * p_c = strstr(p_raw, serialized[i].p_termination_string);
        if (p_c)
        {
            *serialized[i].pp_element = strdup_until(p_raw, p_c);

            p_raw = p_c + strlen(serialized[i].p_termination_string);
            if (p_raw >= p_end)
            {
                break;
            }
        }
        else
        {
            *serialized[i].pp_element = strdup_until(p_raw, p_end);
            break;
        }
    }

    return uri;
}

char * uri_encode(uri_t * p_uri)
{
    size_t len = 0;
    const serialized_uri_t serialized = URI_SERIALIZED(p_uri);
    for (uint32_t i = 0; i < SERIALIZED_URI_ELEM_COUNT; ++i)
    {
        if (*serialized[i].pp_element)
        {
            len += 3 * strlen(*serialized[i].pp_element) + strlen(serialized[i].p_termination_string);
        }
    }
    char * p_retval = MALLOC(len + 1);
    char * p_c = p_retval;
    for (uint32_t i = 0; i < SERIALIZED_URI_ELEM_COUNT; ++i)
    {
        if (*serialized[i].pp_element)
        {
            p_c += strcpy_encoded(p_c, *serialized[i].pp_element);
        }
        if (i < MANDATORY_ELEM_COUNT || (i < SERIALIZED_URI_ELEM_COUNT - 1 && *serialized[i+1].pp_element))
        {
            strcpy(p_c, serialized[i].p_termination_string);
            p_c += strlen(serialized[i].p_termination_string);
        }
    }
    *p_c = '\0';
    return p_retval;
}

void uri_free_members(uri_t * p_uri)
{
    FREE((char *) p_uri->scheme);
    FREE((char *) p_uri->authority);
    FREE((char *) p_uri->path);
    FREE((char *) p_uri->query);
    FREE((char *) p_uri->fragment);
}

uri_t uri_file(const char * p_filename)
{
    uri_t uri;
    memset(&uri, 0, sizeof(uri));
    uri.scheme = STRDUP("file");
    char * p_path = absolute_path(p_filename, path_cwd());

    /* Convert backslashes: */
    for (char * c = &p_path[0]; *c != 0; ++c)
    {
        if (*c == '\\') *c = '/';
    }

    /* Normalize windows drive letters to lower case: */
    if (p_path[1] == ':' && p_path[0] >= 'A' && p_path[0] <= 'Z')
    {
        p_path[0] = 'a' + (p_path[0] - 'A');
    }

    uri.path = p_path;

    return uri;
}

const char * uri_file_extension(const uri_t * p_uri)
{
    if (p_uri && p_uri->path)
    {
        return strrchr(p_uri->path, '.');
    }
    return NULL;
}

const bool uri_is_c_cpp_file(const uri_t * p_uri)
{
    const char * p_extension = uri_file_extension(p_uri);

    static const char * s_valid_extensions[] = {
        ".c", ".cpp", ".cc", ".h", ".hpp", ".hh", ".cxx", ".hxx", ".ino", ".inl"
    };

    if (p_extension)
    {
        for (unsigned i = 0; i < ARRAY_SIZE(s_valid_extensions); ++i)
        {
            if (strcmp(p_extension, s_valid_extensions[i]) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

char * uri_file_encode(const char * p_filename)
{
    uri_t uri = uri_file(p_filename);
    char * p_retval = uri_encode(&uri);
    uri_free_members(&uri);
    return p_retval;
}
