#pragma once
#include <stdbool.h>
#include <stdlib.h>
#if _WIN32
#include <crtdbg.h>
#endif
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define MALLOC(SIZE) malloc(SIZE)
#define CALLOC(SIZE, NUM) calloc(SIZE, NUM)
#define FREE(MEM) free(MEM)
#define REALLOC(MEM, SIZE) realloc(MEM, SIZE)
#if _WIN32
#define STRDUP(STR) _strdup(STR)
#else
#define STRDUP(STR) strdup(STR)
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
