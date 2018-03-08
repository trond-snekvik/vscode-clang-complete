#pragma once

typedef enum
{
    DOXYGEN_ARG_DIRECTION_NONE = 0,
    DOXYGEN_ARG_DIRECTION_IN = (1 << 0),
    DOXYGEN_ARG_DIRECTION_OUT = (1 << 1),
    DOXYGEN_ARG_DIRECTION_IN_OUT = (DOXYGEN_ARG_DIRECTION_IN | DOXYGEN_ARG_DIRECTION_OUT),
} doxygen_arg_direction_t;

typedef const char * doxygen_string_t;

typedef struct
{
    const char * p_name;
    doxygen_arg_direction_t dir;
    doxygen_string_t description;
} doxygen_arg_t;

typedef struct
{
    const char * p_value;
    doxygen_string_t description;
} doxygen_return_t;

typedef struct
{
    doxygen_string_t brief;
    doxygen_string_t details;
    doxygen_arg_t * p_args;
    unsigned arg_count;
    doxygen_return_t * p_returns;
    unsigned return_count;
} doxygen_function_t;

doxygen_function_t * doxygen_function_parse(const char * p_comment);
doxygen_string_t doxygen_description_parse(const char * p_comment);

char * doxygen_to_markdown(doxygen_string_t string, bool skip_newline);

char * doxygen_function_to_markdown(const doxygen_function_t * p_function, bool include_params, bool include_returns);

void doxygen_function_free(doxygen_function_t * p_function);