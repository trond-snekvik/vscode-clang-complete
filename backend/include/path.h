#pragma once
#include <stdbool.h>

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
