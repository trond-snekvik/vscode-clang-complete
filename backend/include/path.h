#pragma once
#include <stdbool.h>
#include "array.h"

#define PATH_PAIR_SCORE_SAME_NAME (1 << 14)
#define PATH_PAIR_SCORE_SUBSTRING (1 << 9)

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
const char * path_filename(const char * p_file);
char * path_filename_no_ext(const char * p_file);

char * path_closest_common_ancestor(const char * p_path1, const char * p_path2);

bool path_equals(const char * p_path1, const char * p_path2);

void path_get_files(const char * p_directory, path_file_cb_t callback, bool recursive, void * p_args);

bool path_last_edit(const char * p_path, time_t * p_time);


/**
 * Give a score for how likely it is that the files are a header/sourcefile pair.
 *
 * @param[in] p_header Header file
 * @param[in] p_source Source file
 *
 * @returns The score for the file pair. The more higher the score, the more likely it is that these files are pairs.
 */
unsigned path_pair_score(const char * p_header, const char * p_source);