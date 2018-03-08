#pragma once
#include <stdbool.h>
#include <stdlib.h>
#if _WIN32
#include <crtdbg.h>
#endif
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define DEBUG_HEAP 0

#if DEBUG_HEAP
#define MALLOC(SIZE) clang_server_malloc(SIZE, __FILE__, __LINE__)
#define CALLOC(SIZE, NUM) clang_server_calloc(SIZE, NUM, __FILE__, __LINE__)
#define STRDUP(STR) clang_server_strdup(STR, __FILE__, __LINE__)
#define FREE(MEM) clang_server_free(MEM)
#define REALLOC(MEM, SIZE) clang_server_realloc(MEM, SIZE, __FILE__, __LINE__)
#else
#define MALLOC(SIZE) malloc(SIZE)
#define CALLOC(SIZE, NUM) calloc(SIZE, NUM)
#define FREE(MEM) free(MEM)
#define REALLOC(MEM, SIZE) realloc(MEM, SIZE)
#if _WIN32
#define STRDUP(STR) _strdup(STR)
#else
#define STRDUP(STR) strdup(STR)
#endif
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct
{
    char ** pp_lines;
    unsigned line_count;
} line_array_t;


line_array_t * line_array(const char * p_string);

void line_array_free(line_array_t * p_line_array);

#if DEBUG_HEAP
void * clang_server_malloc(size_t size, const char * p_file, unsigned line);
void * clang_server_calloc(size_t size, size_t num, const char * p_file, unsigned line);
char * clang_server_strdup(const char * p_original, const char * p_file, unsigned line);
void * clang_server_realloc(void * p_original, size_t size, const char * p_file, unsigned line);
void clang_server_free(void * p_mem);

void heap_dump(const char * p_title);
#endif