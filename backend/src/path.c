#include <stdlib.h>
#include <stdio.h>
#include "path.h"
#include "stack.h"
#include "assert.h"
#include "log.h"
#include "utils.h"
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <Windows.h>
    #include <direct.h>
    #ifndef getcwd
        #define getcwd _getcwd
    #endif
#else
    #include <unistd.h>
    #include <dirent.h>
#endif

static inline bool is_path_slash(char c)
{
    return (c == '/' || c == '\\');
}

static bool is_windows_drive(const char * p_string)
{
    return (((p_string[0] >= 'a' && p_string[0] <= 'z') || (p_string[0] >= 'A' && p_string[0] <= 'Z')) &&
            p_string[1] == ':' &&
            is_path_slash(p_string[2]));
}

char * path_remove_redundant_steps(const char * p_path)
{
    size_t string_length = strlen(p_path);
    if (string_length <= 2)
    {
        return STRDUP(p_path);
    }
    Stack * p_stack = NULL;
    ASSERT(stack_new(&p_stack) == CC_OK);

    char * p_split = STRDUP(p_path);
    ASSERT(stack_push(p_stack, p_split) == CC_OK);

    for (char * p_c = p_split; *p_c; ++p_c)
    {
        if (strchr("/\\", *p_c) != NULL)
        {
            *p_c = '\0';
            char * p_next = p_c + 1;
            if (strstr(p_next, "..") == p_next)
            {
                if (stack_pop(p_stack, NULL) != CC_OK)
                {
                    /* If this fails, the path is invalid */
                    FREE(p_split);
                    return NULL;
                }
            }
            else
            {
                ASSERT(stack_push(p_stack, p_next) == CC_OK);
            }
        }
    }

    char * p_return = CALLOC(1, string_length + 1);

    char * p_dst = p_return;
    StackIter iter;
    stack_iter_init(&iter, p_stack);
    void * p_elem;
    ASSERT(stack_iter_next(&iter, &p_elem) == CC_OK);

    while (true)
    {
        size_t elem_len = strlen(p_elem);
        strcpy(p_dst, p_elem);
        p_dst += elem_len;

        if (stack_iter_next(&iter, &p_elem) != CC_OK)
        {
            break;
        }

        *p_dst = '/';
        p_dst++;
    }
    *p_dst = '\0';
    stack_destroy(p_stack);
    FREE(p_split);
    return p_return;
}

char * normalize_path(const char * p_path)
{
    char * p_duplicate_path = STRDUP(p_path);
    if (is_windows_drive(p_duplicate_path) && (p_duplicate_path[0] >= 'a' && p_duplicate_path[0] <= 'z'))
    {
        p_duplicate_path[0] += 'A' - 'a';
    }
    for (char * p_c = &p_duplicate_path[0]; *p_c; p_c++)
    {
        if (*p_c == '\\')
        {
            *p_c = '/';
        }
    }
    char * p_retval = path_remove_redundant_steps(p_duplicate_path);
    FREE(p_duplicate_path);
    return p_retval;
}

bool is_absolute_path(const char * p_path)
{
    return (p_path[0] == '/' || is_windows_drive(p_path));
}

char * absolute_path(const char * p_path, const char * p_root)
{
    ASSERT(p_path);
    ASSERT(p_root);
    if (is_absolute_path(p_path))
    {
        return normalize_path(p_path);
    }
    else
    {
        size_t root_len = strlen(p_root);
        size_t path_len = strlen(p_path);
        char * p_absolute = MALLOC(path_len + root_len + 2);
        char * p_dst = &p_absolute[0];
        strcpy(p_dst, p_root);
        p_dst = &p_dst[root_len - 1];
        if (!is_path_slash(*p_dst))
        {
            p_dst++;
            *p_dst = '/';
        }
        p_dst++;
        strcpy(p_dst, p_path);
        p_dst[path_len] = 0;

        char * p_retval = normalize_path(p_absolute);
        FREE(p_absolute);
        return p_retval;
    }
}

const char * path_cwd(void)
{
    static char * p_cwd = NULL;
    if (p_cwd)
    {
        return p_cwd;
    }
    else
    {
        p_cwd = MALLOC(FILENAME_MAX);
        return getcwd(p_cwd, FILENAME_MAX);
    }
}

bool path_is_directory(const char * p_path)
{
    struct stat file_stat;
    return ((stat(p_path, &file_stat) == 0) && (file_stat.st_mode & S_IFDIR));
}

char * path_directory(const char * p_file)
{
    char * p_normalized = normalize_path(p_file);

    if (path_is_directory(p_normalized))
    {
        return p_normalized;
    }
    else
    {
        char * p_last_slash = strrchr(p_normalized, '/');
        if (p_last_slash)
            *p_last_slash = '\0';
        return p_normalized;
    }
}
const char * path_filename(const char * p_file)
{
    const char * p_last_forward_slash = strrchr(p_file, '/');
    const char * p_last_backward_slash = strrchr(p_file, '/');
    if (!p_last_backward_slash && !p_last_backward_slash)
    {
        return p_file;
    }
    else
    {
        return max(p_last_backward_slash, p_last_forward_slash) + 1;
    }
}

char * path_filename_no_ext(const char * p_file)
{
    char * p_filename = STRDUP(path_filename(p_file));
    char * p_extension = strchr(p_filename, '.');
    if (p_extension)
    {
        *p_extension = '\0';
    }
    return p_filename;
}

char * path_closest_common_ancestor(const char * p_path1, const char * p_path2)
{
    char * p_abs_path1 = absolute_path(p_path1, path_cwd());
    char * p_abs_path2 = absolute_path(p_path2, path_cwd());
    int first_diff = strcmp(p_abs_path1, p_abs_path2);
    FREE(p_abs_path2); // will operate on path1 from now

    if (first_diff != 0)
    {
        p_abs_path1[abs(first_diff)] = '\0';
    }
    return p_abs_path1;
}

bool path_equals(const char * p_path1, const char * p_path2)
{
    char * p_abs_path1 = absolute_path(p_path1, path_cwd());
    char * p_abs_path2 = absolute_path(p_path2, path_cwd());
    size_t diff = strcmp(p_abs_path1, p_abs_path2);
    free(p_abs_path1);
    free(p_abs_path2);
    return (diff == 0);
}

void path_get_files(const char * p_directory, path_file_cb_t callback, bool recursive, void * p_args)
{
#ifdef _WIN32
    LOG("Path get files for %s\n", p_directory);
    char match_string[MAX_PATH];
    sprintf(match_string, "%s/*", p_directory);
    WIN32_FIND_DATA file_data;
    HANDLE handle = FindFirstFile(match_string, &file_data);
    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (strcmp(file_data.cFileName, ".") != 0 && strcmp(file_data.cFileName, "..") != 0)
            {
                path_kind_t kind = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? PATH_KIND_DIRECTORY : PATH_KIND_FILE;

                char full_path[MAX_PATH];
                sprintf(full_path, "%s/%s", p_directory, file_data.cFileName);

                if (kind == PATH_KIND_DIRECTORY && recursive)
                {
                    path_get_files(full_path, callback, true, p_args);
                }
                callback(full_path, kind, p_args);
            }
        } while (FindNextFile(handle, &file_data));
        FindClose(handle);
    }
    else
    {
        LOG("FAILED LOOKING UP %s\n", p_directory);
    }
#else
    DIR * p_dir = opendir(p_directory);
    if (p_dir)
    {
        for (struct dirent * p_ent = readdir(p_dir);
             p_ent != NULL;
             p_ent = readdir(p_dir))
        {
            callback(p_ent->d_name, PATH_KIND_UNKNOWN, p_args);
        }
        closedir (dir);
    }
#endif
}

#ifdef _WIN32
bool path_last_edit(const char * p_path, time_t * p_time)
{
    HANDLE hFile = CreateFile(p_path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        FILETIME file_time;
        if (GetFileTime(hFile, NULL, NULL, &file_time))
        {
            uint64_t long_time = file_time.dwLowDateTime + ((uint64_t) file_time.dwHighDateTime << 32);
            *p_time = (long_time - 116444736000000000) / 10000000;
        }
    }
    CloseHandle(hFile);
    return (hFile != INVALID_HANDLE_VALUE);
}
#endif

unsigned path_pair_score(const char * p_header, const char * p_source)
{
    char * p_header_filename = path_filename_no_ext(p_header);
    char * p_source_filename = path_filename_no_ext(p_source);
    size_t header_len = strlen(p_header_filename);
    size_t source_len = strlen(p_source_filename);

    unsigned score = 0;
    if (header_len == source_len)
    {
        if (memcmp(p_header_filename, p_source_filename, header_len) == 0)
        {
            score = PATH_PAIR_SCORE_SAME_NAME;
        }
    }
    else if (strstr(p_header_filename, p_source_filename) || strstr(p_source_filename, p_header_filename))
    {
        score = PATH_PAIR_SCORE_SUBSTRING;
    }

    /* Common ancestor length is the tie breaker */
    char * p_common_ancestor = path_closest_common_ancestor(p_header, p_source);
    score += strlen(p_common_ancestor);
    FREE(p_common_ancestor);
    FREE(p_header_filename);
    FREE(p_source_filename);

    return score;
}
