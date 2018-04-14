#pragma once
#include <stdbool.h>
#include <clang-c/Index.h>
#include "structures.h"

typedef struct
{
    struct CXUnsavedFile * p_list;
    unsigned count;
} unsaved_files_t;

void unsaved_files_init(void);
const unsaved_files_t * unsaved_files_get(void);
void unsaved_file_set(const char * p_filename, const char * p_contents);
bool unsaved_file_remove(const char * p_filename);
void unsaved_file_patch(const char * p_filename, const char * p_new_contents, const position_t * p_start_pos, size_t old_len);
void unsaved_files_release(void);