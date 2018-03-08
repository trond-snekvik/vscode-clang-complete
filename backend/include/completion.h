#pragma once
#include <stdbool.h>
#include "clang-c/Index.h"

#define STRING_UNKNOWN "unknown"

typedef enum
{
    COMPLETION_KIND_FIELD,
    COMPLETION_KIND_STRUCT,
    COMPLETION_KIND_UNION,
    COMPLETION_KIND_CLASS,
    COMPLETION_KIND_ENUM,
    COMPLETION_KIND_ENUM_CONSTANT,
    COMPLETION_KIND_MACRO,
    COMPLETION_KIND_FUNCTION,
    COMPLETION_KIND_VARIABLE,
    COMPLETION_KIND_PARAMETER,
    COMPLETION_KIND_NAMESPACE,
    COMPLETION_KIND_TYPEDEF,
    COMPLETION_KIND_LANGUAGE_TOKEN,
    COMPLETION_KIND_UNKNOWN,
} completion_kind_t;

typedef enum
{
    COMPLETION_REFERENCE_KIND_DEFINITION,
    COMPLETION_REFERENCE_KIND_REFERENCE,
} completion_reference_kind_t;

typedef enum
{
    COMPLETION_AVAILABILITY_AVAILABLE,
    COMPLETION_AVAILABILITY_DEPRECATED,
    COMPLETION_AVAILABILITY_NOT_AVAILABLE,
    COMPLETION_AVAILABILITY_NOT_ACCESSIBLE,
} completion_availability_t;

typedef struct
{
    const char * p_filename;
    unsigned line;
    unsigned column;
} completion_location_t;

typedef struct
{
    const char * p_name;
    const char * p_type;
    const char * p_documentation;
    completion_location_t location;
    completion_kind_t kind;
} completion_definition_t;

typedef struct
{
    const char * p_name;
    const char * p_type;
    const char * p_default_value;
} completion_fuction_param_t;

typedef struct
{
    completion_definition_t definition;
    completion_fuction_param_t * p_params;
    unsigned param_count;
    const char * p_return_type;
} completion_function_definition_t;

typedef struct
{
    const char * p_typed_text;
    const char * p_inserted_text;
    const char * p_display_value;
    const char * p_documentation;
    const char * p_type;
    completion_kind_t kind;
    unsigned priority; ///< Lower is better
    completion_availability_t availability;
    unsigned index;
} completion_result_t;

typedef struct CXUnsavedFile unsaved_file_t;

typedef void (*completion_result_callback_t)(const completion_result_t * p_result, unsigned total, void * p_args);
typedef void (*completion_reference_callback_t)(completion_reference_kind_t kind, const completion_location_t * p_location, unsigned index, void * p_args);

void completion_init(void);


bool completion_set_file(const char * p_filename,
                         const char * const * p_defines,
                         unsigned define_count,
                         const char * const * p_includes,
                         unsigned include_count,
                         unsaved_file_t * p_unsaved_files,
                         unsigned unsaved_file_count);


const unsigned completion_get(unsigned line,
                              unsigned column,
                              unsaved_file_t * p_unsaved_files,
                              unsigned unsaved_file_count,
                              completion_result_callback_t callback,
                              void * p_args);

const completion_function_definition_t * completion_function_definition_get(unsigned line,
                                                            unsigned column,
                                                            unsaved_file_t * p_unsaved_files,
                                                            unsigned unsaved_file_count);

void completion_function_definition_free(const completion_function_definition_t * p_function_definition);

const completion_definition_t * completion_definition_get(unsigned line,
                                                          unsigned column,
                                                          unsaved_file_t * p_unsaved_files,
                                                          unsigned unsaved_file_count);
void completion_definition_free(const completion_definition_t * p_definition);


unsigned completion_references_get(unsigned line,
                                   unsigned column,
                                   unsaved_file_t * p_unsaved_files,
                                   unsigned unsaved_file_count,
                                   completion_reference_callback_t callback,
                                   void * p_args);

static inline const char * completion_kind_string_get(completion_kind_t kind)
{
    static const char * sp_kinds[] =
    {
        "field",
        "struct",
        "union",
        "class",
        "enum",
        "enum_constant",
        "macro",
        "function",
        "variable",
        "parameter",
        "namespace",
        "typedef",
        "language_token"
    };
    if (kind < sizeof(sp_kinds) / sizeof(sp_kinds[0]))
    {
        return sp_kinds[kind];
    }
    else
    {
        return STRING_UNKNOWN;
    }
}

static inline const char * completion_availability_string_get(completion_availability_t availability)
{
    static const char * sp_availabilies[] =
    {
        "available",
        "deprecated",
        "not_available",
        "not_accessible",
    };
    if (availability < sizeof(sp_availabilies) / sizeof(sp_availabilies[0]))
    {
        return sp_availabilies[availability];
    }
    else
    {
        return STRING_UNKNOWN;
    }
}

static inline const char * completion_reference_kind_string_get(completion_reference_kind_t ref_kind)
{
    static const char * sp_ref_kinds[] =
    {
        "definition",
        "reference"
    };
    if (ref_kind < sizeof(sp_ref_kinds) / sizeof(sp_ref_kinds[0]))
    {
        return sp_ref_kinds[ref_kind];
    }
    else
    {
        return STRING_UNKNOWN;
    }
}