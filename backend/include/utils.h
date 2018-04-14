#pragma once
#include <stdbool.h>
#include <stdlib.h>
#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>
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

typedef struct thread thread_t;

typedef struct
{
#ifdef _WIN32
    CRITICAL_SECTION critical_section;
#else
    #error "Not implemented."
#endif
} mutex_t;

typedef struct
{
#ifdef _WIN32
    HANDLE sem;
#else
    #error "Not implemented."
#endif
} semaphore_t;

typedef struct
{
#ifdef _WIN32
    LONG value;
#endif
} atomic_counter_t;

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