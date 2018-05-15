#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
// #include "utils.h"

#define LOG_FILE    "log.txt"
#define OUT_FILE    "out.txt"

#define LOG_ERASE() file_erase(LOG_FILE)
#define OUT_ERASE() file_erase(OUT_FILE)

#define LOG_BUFFER_SIZE (1024 * 1000)
#if _DEBUG
#define LOG(fmt, ...)                     \
    do                                    \
    {                                     \
        extern char g_log_buf[];                   \
        sprintf(g_log_buf, fmt, ##__VA_ARGS__); \
        safe_puts(LOG_FILE, g_log_buf);\
    } while (0)

#define OUTPUT(_str)                  \
    do                                    \
    {                                     \
        printf("%s", _str);                   \
        fflush(stdout); \
        safe_puts(OUT_FILE, _str);\
    } while (0)
#else
#define LOG(fmt, ...)

#define OUTPUT(_str)                  \
    do                                \
    {                                 \
        printf("%s", _str);           \
        fflush(stdout);               \
    } while (0)
#endif

void log_init(void);
void safe_puts(const char * p_filename, const char * p_string);
void json_rpc_log(const char * p_message);
void assert_handler(const char * p_file, unsigned line);

static inline void file_erase(const char * p_filename)
{
    FILE * p_file = fopen(p_filename, "w");
    if (p_file)
    {
        fclose(p_file);
    }
}

#define ASSERT(x) if (!(x)) assert_handler(__FILE__, __LINE__)