#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>
#endif

#include "log.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define MALLOC(SIZE) utils_malloc(SIZE, __FILE__, __LINE__)
#define CALLOC(SIZE, NUM) utils_calloc(SIZE, NUM, __FILE__, __LINE__)
#define FREE(MEM) free(MEM)
#define REALLOC(MEM, SIZE) utils_realloc(MEM, SIZE, __FILE__, __LINE__)
#if _WIN32
#define STRDUP(STR) utils_strdup(STR, __FILE__, __LINE__)
#else
#define STRDUP(STR) strdup(STR)
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

static inline void * utils_malloc(size_t size, const char * p_filename, unsigned line)
{
    void * p_data = malloc(size);
    if (p_data == NULL)
    {
        LOG("Malloc failed at %s:%u (size: %u)\n", p_filename, line, size);
        ASSERT(false);
    }
    return p_data;
}
static inline void * utils_calloc(size_t size, size_t num, const char * p_filename, unsigned line)
{
    void * p_data = calloc(size, num);
    if (p_data == NULL)
    {
        LOG("Calloc failed at %s:%u (size: %u, num: %u)\n", p_filename, line, size, num);
        ASSERT(false);
    }
    return p_data;
}
static inline void * utils_realloc(void * p_data, size_t size, const char * p_filename, unsigned line)
{
    void * p_new_data = realloc(p_data, size);
    if (p_new_data == NULL)
    {
        LOG("Realloc failed at %s:%u (new size: %u)\n", p_filename, line, size);
        ASSERT(false);
    }
    return p_new_data;
}
static inline char * utils_strdup(const char * p_string, const char * p_filename, unsigned line)
{
    char * p_data = _strdup(p_string);
    if (p_data == NULL && p_string != NULL)
    {
        LOG("Malloc failed at %s:%u (string: %s)\n", p_filename, line, p_string);
        ASSERT(false);
    }
    return p_data;
}

typedef struct
{
    char ** pp_lines;
    unsigned line_count;
} line_array_t;

typedef struct thread thread_t;

#ifdef _WIN32
typedef struct
{
    CRITICAL_SECTION critical_section;
} mutex_t;

typedef struct
{
    HANDLE sem;
} semaphore_t;

typedef struct
{
    LONG value;
} atomic_counter_t;

typedef DWORD profile_time_t;
#endif

typedef struct
{
    mutex_t mut;
    mutex_t owner_mut;
    unsigned users;
} shared_resource_t;

typedef enum
{
    THREAD_PRIO_LOW,
    THREAD_PRIO_NORMAL,
    THREAD_PRIO_HIGH,
} thread_priority_t;


typedef void (*thread_function_t)(void * p_args);

line_array_t * line_array(const char * p_string);

void line_array_free(line_array_t * p_line_array);


thread_t * thread_start(thread_function_t function, void * p_args, thread_priority_t priority);

void thread_join(thread_t * p_thread);
void thread_cancel(thread_t * p_thread);

void mutex_init(mutex_t *  p_mut);
void mutex_take(mutex_t * p_mut);
bool mutex_try_take(mutex_t * p_mut);
void mutex_release(mutex_t * p_mut);
void mutex_free(mutex_t * p_mut);

void semaphore_init(semaphore_t * p_sem, unsigned max_count);
void semaphore_wait(semaphore_t * p_sem);
void semaphore_signal(semaphore_t * p_sem);

int atomic_get_and_add(atomic_counter_t * p_counter);
int atomic_get_and_sub(atomic_counter_t * p_counter);

void shared_resource_init(shared_resource_t * p_resource);
void shared_resource_borrow(shared_resource_t * p_resource);
void shared_resource_release(shared_resource_t * p_resource);
void shared_resource_lock(shared_resource_t * p_resource);
void shared_resource_unlock(shared_resource_t * p_resource);

bool string_fuzzy_match(const char * p_string, const char * p_pattern);

profile_time_t profile_start(void);
unsigned profile_end(profile_time_t start_time);
