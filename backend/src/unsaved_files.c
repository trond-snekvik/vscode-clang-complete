#include "unsaved_files.h"
#include "hashtable.h"
#include "source_file.h"
#include "log.h"

static HashTable * mp_unsaved_files;
static shared_resource_t m_resource;
static mutex_t m_mutex;

void unsaved_files_init(void)
{
    ASSERT(hashtable_new(&mp_unsaved_files) == CC_OK);
    shared_resource_init(&m_resource);
    mutex_init(&m_mutex);
}

unsaved_files_t * unsaved_files_get(void)
{
    mutex_take(&m_mutex);
    unsaved_files_t * p_unsaved_files = MALLOC(sizeof(unsaved_files_t));

    size_t size = hashtable_size(mp_unsaved_files);

    if (size > 0)
    {
        p_unsaved_files->p_list = MALLOC(sizeof(struct CXUnsavedFile) * size);
        p_unsaved_files->count = 0;

        HashTableIter it;
        TableEntry * p_entry = NULL;
        hashtable_iter_init(&it, mp_unsaved_files);
        while (hashtable_iter_next(&it, &p_entry) == CC_OK)
        {
            source_file_t * p_value = (source_file_t *) p_entry->value;
            if (p_value->unsaved)
            {
                p_unsaved_files->p_list[p_unsaved_files->count].Filename = (const char *) STRDUP(p_entry->key);
                p_unsaved_files->p_list[p_unsaved_files->count].Contents = STRDUP(p_value->p_contents);
                p_unsaved_files->p_list[p_unsaved_files->count].Length   = (unsigned long) p_value->size;
                ASSERT(strlen(p_unsaved_files->p_list[p_unsaved_files->count].Contents) == p_value->size);
                p_unsaved_files->count++;
            }
        }

        if (p_unsaved_files->count == 0)
        {
            FREE(p_unsaved_files->p_list);
            p_unsaved_files->p_list = NULL;
        }
    }
    else
    {
        p_unsaved_files->p_list = NULL;
        p_unsaved_files->count = 0;
    }
    mutex_release(&m_mutex);
    return p_unsaved_files;
}

void unsaved_file_set(const char * p_filename, const char * p_contents)
{
    mutex_take(&m_mutex);
    LOG("Unsaved file set: %s\n", p_filename);

    source_file_t * p_file;
    if (hashtable_get(mp_unsaved_files, (void *) p_filename, &p_file) == CC_OK)
    {
        LOG("\t->Replace\n");
        source_file_contents_set(p_file, p_contents);
    }
    else
    {
        LOG("\t->Add\n");
        ASSERT(hashtable_add(mp_unsaved_files, STRDUP(p_filename), source_file_create(p_contents)) == CC_OK);
    }
    mutex_release(&m_mutex);
}

void unsaved_file_patch(const char * p_filename, const char * p_new_contents, const position_t * p_start_pos, size_t old_len)
{
    mutex_take(&m_mutex);
    LOG("Patching unsaved file %s...\n", p_filename);
    source_file_t * p_file = NULL;
    ASSERT(hashtable_get(mp_unsaved_files, (void *) p_filename, &p_file) == CC_OK);
    LOG("Found unsaved file...\n");
    source_file_patch(p_file, p_new_contents, p_start_pos, old_len);
    mutex_release(&m_mutex);
}

bool unsaved_file_remove(const char * p_filename)
{
    mutex_take(&m_mutex);
    LOG("Remove unsaved file %s\n", p_filename);
    source_file_t * p_file;
    bool success = (hashtable_remove(mp_unsaved_files, (void *) p_filename, &p_file) == CC_OK);
    if (success)
    {
        source_file_free(p_file);
    }
    mutex_release(&m_mutex);
    return success;
}

void unsaved_files_release(unsaved_files_t * p_unsaved_files)
{
    for (unsigned i = 0; i < p_unsaved_files->count; ++i)
    {
        FREE((char *) p_unsaved_files->p_list[i].Contents);
        FREE((char *) p_unsaved_files->p_list[i].Filename);
    }
    FREE(p_unsaved_files->p_list);
    FREE(p_unsaved_files);
}

void unsaved_files_free(void)
{
    mutex_take(&m_mutex);
    if (hashtable_size(mp_unsaved_files) > 0)
    {
        HashTableIter it;
        TableEntry * p_entry = NULL;
        hashtable_iter_init(&it, mp_unsaved_files);
        while (hashtable_iter_next(&it, &p_entry) == CC_OK)
        {
            source_file_t * p_value;
            ASSERT(hashtable_iter_remove(&it, &p_value) == CC_OK);
            source_file_free(p_value);
        }
    }
    hashtable_destroy(mp_unsaved_files);
    mutex_release(&m_mutex);
}