#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <clang-c/Index.h>

#include "source_file.h"


typedef struct
{
    CXTranslationUnit translation_unit;
    source_file_t * p_file;
} context_t;



void context_init(void);

context_t * context_create(const char * p_filename, const char ** pp_compile_args; uint32_t compile_arg_count);

context_t * context_get(const char * p_filename);