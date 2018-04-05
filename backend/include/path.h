#pragma once
#include <stdbool.h>
#include "array.h"

typedef enum
{
    PATH_KIND_FILE,
    PATH_KIND_DIRECTORY,
    PATH_KIND_UNKNOWN
} path_kind_t;

typedef void (*path_file_cb_t)(const char * p_filename, path_kind_t kind, void * p_args);

char * path_remove_redundant_steps(const char * p_path);

char * normalize_path(const char * p_path);

bool is_absolute_path(const char * p_path);

char * absolute_path(const char * p_path, const char * p_root);

char * get_source_file(char * p_header_file);

const char * path_cwd(void);

bool path_is_directory(const char * p_path);

char * path_directory(const char * p_file);

char * path_closest_common_ancestor(const char * p_path1, const char * p_path2);

bool path_equals(const char * p_path1, const char * p_path2);

void path_get_files(const char * p_directory, path_file_cb_t callback, bool recursive, void * p_args);