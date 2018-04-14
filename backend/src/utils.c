#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "log.h"
#include "hashtable.h"

#ifdef _WIN32
    #include <Windows.h>
    #define STACK_SIZE 4096
#else
    #include <pthread.h>
#endif

struct thread
{
    thread_function_t func;
    void * p_args;
#ifdef _WIN32
    DWORD id;
    HANDLE handle;
#else
    pthread_t pthread;
#endif
};


line_array_t * line_array(const char * p_string)
{
    if (!p_string)
    {
        return NULL;
    }

    line_array_t * p_result = MALLOC(sizeof(line_array_t));
    p_result->line_count = 1;
    for (const char * p_c = &p_string[0]; *p_c; p_c++)
    {
        if (*p_c == '\n')
        {
            p_result->line_count++;
        }
    }

    p_result->pp_lines = MALLOC(sizeof(char *) * p_result->line_count);

    const char * p_c = &p_string[0];

    for (unsigned i = 0; i < p_result->line_count; ++i)
    {
        unsigned len = strlen(p_c);
        p_result->pp_lines[i] = MALLOC(len + 1);
        memcpy(p_result->pp_lines[i], p_c, len);
        p_result->pp_lines[i][len] = '\0';
        p_c += len + 1;
    }
    return p_result;
}

void line_array_free(line_array_t * p_line_array)
{
    if (p_line_array)
    {
        for (unsigned i = 0; i < p_line_array->line_count; ++i)
        {
           FREE(p_line_array->pp_lines[i]);
        }
        FREE(p_line_array->pp_lines);
        FREE(p_line_array);
    }
}

#ifdef _WIN32
static DWORD WINAPI win_thread(void * p_args)
{
    thread_t * p_thread = p_args;
    p_thread->func(p_thread->p_args);
    return 0;
}

thread_t * thread_start(thread_function_t function, void * p_args, thread_priority_t priority)
{
    thread_t * p_thread = MALLOC(sizeof(thread_t));
    ASSERT(p_thread);
    p_thread->func = function;
    p_thread->p_args = p_args;

    int win32_priority[] = {
        [THREAD_PRIO_LOW]    = THREAD_PRIORITY_LOWEST,
        [THREAD_PRIO_NORMAL] = THREAD_PRIORITY_NORMAL,
        [THREAD_PRIO_HIGH]   = THREAD_PRIORITY_ABOVE_NORMAL,
    };

    p_thread->handle = CreateThread(NULL, STACK_SIZE, win_thread, p_thread, CREATE_SUSPENDED, &p_thread->id);
    SetThreadPriority(p_thread->handle, win32_priority[priority]);
    ResumeThread(p_thread->handle);
    return p_thread;
}

void thread_join(thread_t * p_thread)
{
    WaitForSingleObject(p_thread->handle, INFINITE);
}

void mutex_init(mutex_t *  p_mut)
{
    InitializeCriticalSection(&p_mut->critical_section);
}

void mutex_take(mutex_t * p_mut)
{
    EnterCriticalSection(&p_mut->critical_section);
}

bool mutex_try_take(mutex_t * p_mut)
{
    return TryEnterCriticalSection(&p_mut->critical_section);
}

void mutex_release(mutex_t * p_mut)
{
    LeaveCriticalSection(&p_mut->critical_section);
}

void mutex_free(mutex_t * p_mut)
{
}

void semaphore_init(semaphore_t * p_sem, unsigned max_count)
{
    p_sem->sem = CreateSemaphore(NULL, 0, max_count, NULL);
}

void semaphore_wait(semaphore_t * p_sem)
{
    WaitForSingleObject(p_sem->sem, 0);
}

void semaphore_signal(semaphore_t * p_sem)
{
    ReleaseSemaphore(p_sem->sem, 1, NULL);
}

int atomic_get_and_add(atomic_counter_t * p_flag)
{
    return (int) InterlockedAdd(&p_flag->value, 1);
}

int atomic_get_and_sub(atomic_counter_t * p_flag)
{
    return (int) InterlockedAdd(&p_flag->value, -1);
}

void shared_resource_init(shared_resource_t * p_resource)
{
    mutex_init(&p_resource->mut);
    p_resource->user_event = CreateEvent(NULL, true, true, NULL);
    p_resource->owner_event = CreateEvent(NULL, true, true, NULL);
    p_resource->users = 0;
}

void shared_resource_borrow(shared_resource_t * p_resource)
{
    mutex_take(&p_resource->mut);
    if (p_resource->users++ == 0)
    {
        ResetEvent(p_resource->user_event);
    }
    mutex_release(&p_resource->mut);
    WaitForSingleObject(p_resource->owner_event, 0);
}

void shared_resource_release(shared_resource_t * p_resource)
{
    mutex_take(&p_resource->mut);
    ASSERT(p_resource->users > 0);
    if (--p_resource->users == 0)
    {
        SetEvent(p_resource->user_event);
    }
    mutex_release(&p_resource->mut);
}

void shared_resource_lock(shared_resource_t * p_resource)
{
    ResetEvent(p_resource->owner_event);
    WaitForSingleObject(p_resource->user_event, 0);
}

void shared_resource_unlock(shared_resource_t * p_resource)
{
    SetEvent(p_resource->owner_event);
}

#else
static void * posix_thread(void * p_args)
{
    thread_t * p_thread = p_args;
    p_thread->func(p_thread->p_args);
    return NULL;
}
#endif

