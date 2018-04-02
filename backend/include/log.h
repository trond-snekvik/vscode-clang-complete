#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define LOG_FILE    "log.txt"
#define OUT_FILE    "out.txt"

#define LOG_ERASE() fclose(fopen(LOG_FILE, "w"))
#define OUT_ERASE() fclose(fopen(OUT_FILE, "w"))

#define LOG_BUFFER_SIZE (1024 * 1000)
#if _DEBUG
#define LOG(fmt, ...)                     \
    do                                    \
    {                                     \
        extern char g_log_buf[];                   \
        sprintf(g_log_buf, fmt, ##__VA_ARGS__); \
        FILE *f = fopen(LOG_FILE, "a");   \
        ASSERT(f); \
        fputs(g_log_buf, f);   \
        fclose(f);                        \
    } while (0)

#define OUTPUT(_str)                  \
    do                                    \
    {                                     \
        printf("%s", _str);                        \
        fflush(stdout); \
        FILE *f = fopen(OUT_FILE, "a");   \
        fputs(_str, f);                    \
        fclose(f);                        \
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

void json_rpc_log(const char * p_message);
void assert_handler(const char * p_file, unsigned line);

#define ASSERT(x) if (!(x)) assert_handler(__FILE__, __LINE__)