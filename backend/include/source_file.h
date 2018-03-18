#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "structures.h"

typedef struct
{
    char * p_contents;
    size_t size;
    bool unsaved;
} source_file_t;



source_file_t * source_file_create(const char * p_contents);
void source_file_contents_set(source_file_t * p_source_file, const char * p_new_contents);

void source_file_free(source_file_t * p_source_file);
void source_file_patch(source_file_t * p_source_file, const char * p_new_contents, const position_t * p_start_pos, size_t old_len);

// bool source_file_unload(const source_file_t * p_source_file);

// const char * source_file_contents_get(source_file_t * p_source_file);
// const char * source_file_line_get(const source_file_t * p_source_file, uint32_t line);

// bool source_file_edit_apply(const source_file_t * p_source_file, const text_edit_t * p_edit);


