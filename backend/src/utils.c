#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "log.h"
#include "hashtable.h"

struct
{
    uint64_t allocs;
    uint64_t frees;
    HashTable * p_table_total;
    HashTable * p_table_new;
} m_heap;

typedef struct
{
    const char * p_file;
    unsigned line;
} heap_alloc_t;

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

#if DEBUG_HEAP
static void alloc_record(void * p_memory, const char * p_file, unsigned line)
{
    if (!m_heap.p_table_total)
    {
        HashTableConf conf;
        cc_hashtable_conf_init(&conf);
        conf.hash = POINTER_HASH;
        ASSERT(cc_hashtable_new_conf(&conf, &m_heap.p_table_total) == CC_OK);
        ASSERT(cc_hashtable_new_conf(&conf, &m_heap.p_table_new) == CC_OK);
    }
    heap_alloc_t * p_alloc = malloc(sizeof(heap_alloc_t));
    ASSERT(p_alloc);
    p_alloc->p_file = p_file;
    p_alloc->line = line;
    ASSERT(cc_hashtable_add(m_heap.p_table_total, p_memory, p_alloc) == CC_OK);
    ASSERT(cc_hashtable_add(m_heap.p_table_new, p_memory, p_alloc) == CC_OK);
}

static void alloc_remove(void * p_memory)
{
    cc_hashtable_remove(m_heap.p_table_total, p_memory, NULL);
    cc_hashtable_remove(m_heap.p_table_new, p_memory, NULL);
}

void * clang_server_malloc(size_t size, const char * p_file, unsigned line)
{
    m_heap.allocs++;
    void * p_alloc = malloc(size);
    ASSERT(p_alloc);
    alloc_record(p_alloc, p_file, line);
    return p_alloc;
}

void * clang_server_calloc(size_t size, size_t num, const char * p_file, unsigned line)
{
    m_heap.allocs++;
    void * p_alloc = calloc(size, num);
    ASSERT(p_alloc);
    alloc_record(p_alloc, p_file, line);
    return p_alloc;
}

char * clang_server_strdup(const char * p_original, const char * p_file, unsigned line)
{
    m_heap.allocs++;
    char * p_alloc = _strdup(p_original);
    ASSERT(p_alloc);
    alloc_record(p_alloc, p_file, line);
    return p_alloc;
}

void * clang_server_realloc(void * p_original, size_t size, const char * p_file, unsigned line)
{
    m_heap.allocs++;
    if (p_original)
    {
        m_heap.frees++;
        alloc_remove(p_original);
    }
    void * p_alloc = realloc(p_original, size);
    ASSERT(p_alloc);
    alloc_record(p_alloc, p_file, line);
    return p_alloc;
}

void clang_server_free(void * p_mem)
{
    if (p_mem)
    {
        m_heap.frees++;
        alloc_remove(p_mem);
    }
    free(p_mem);
}

void heap_dump(const char * p_title)
{
    LOG("HEAP @ %s: %lld (%lld, %lld)\n", p_title, (int64_t) m_heap.allocs - (int64_t) m_heap.frees, m_heap.allocs, m_heap.frees);
    if (cc_hashtable_size(m_heap.p_table_new) > 0)
    {
        LOG("New entries:\n");
        HashTableIter iter;
        cc_hashtable_iter_init(&iter, m_heap.p_table_new);
        TableEntry * p_entry;
        while (cc_hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            ASSERT(p_entry);
            heap_alloc_t * p_alloc = p_entry->value;
            ASSERT(p_alloc);
            ASSERT(p_alloc->p_file);
            LOG("\t%s:%u\n", p_alloc->p_file, p_alloc->line);
        }
        LOG("--------------------\n");
        cc_hashtable_remove_all(m_heap.p_table_new);
    }
}
#endif
