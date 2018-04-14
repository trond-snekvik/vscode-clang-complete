#include "unsaved_files.h"
#include "hashtable.h"
#include "source_file.h"
#include "log.h"

static HashTable * mp_unsaved_files;
static unsaved_files_t m_unsaved_files;
static shared_resource_t m_resource;
static mutex_t m_mutex;

void unsaved_files_init(void)
{
    ASSERT(hashtable_new(&mp_unsaved_files) == CC_OK);
    shared_resource_init(&m_resource);
    mutex_init(&m_mutex);
}

const unsaved_files_t * unsaved_files_get(void)
{
    shared_resource_borrow(&m_resource);
    mutex_take(&m_mutex);
    if (m_unsaved_files.p_list == NULL)
    {
        size_t size = hashtable_size(mp_unsaved_files);

        if (size > 0)
        {
            m_unsaved_files.p_list = MALLOC(sizeof(struct CXUnsavedFile) * size);
            struct CXUnsavedFile * p_file = &m_unsaved_files.p_list[0];
            HashTableIter it;
            TableEntry * p_entry = NULL;
            hashtable_iter_init(&it, mp_unsaved_files);
            while (hashtable_iter_next(&it, &p_entry) == CC_OK)
            {
                source_file_t * p_value = (source_file_t *) p_entry->value;
                p_file->Filename = (const char *) p_entry->key;
                p_file->Contents = p_value->p_contents;
                p_file->Length   = (unsigned long) p_value->size;
                if (p_file->Length != strlen(p_file->Contents))
                {
                    LOG("->\tFile size %u != %u\n", p_file->Length, strlen(p_file->Contents));
                }
                p_file++;
            }
            m_unsaved_files.count = size;
        }
        else
        {
            m_unsaved_files.p_list = NULL;
            m_unsaved_files.count = 0;
        }
    }
    mutex_release(&m_mutex);
    return &m_unsaved_files;
}

static void unsaved_files_invalidate(void)
{
    FREE(m_unsaved_files.p_list);
    m_unsaved_files.p_list = NULL;
    m_unsaved_files.count = 0;
}

void unsaved_file_set(const char * p_filename, const char * p_contents)
{
    shared_resource_lock(&m_resource);

    source_file_t * p_file;
    if (hashtable_get(mp_unsaved_files, (void *) p_filename, &p_file) == CC_OK)
    {
        source_file_contents_set(p_file, p_contents);
    }
    else
    {
        ASSERT(hashtable_add(mp_unsaved_files, STRDUP(p_filename), source_file_create(p_contents)) == CC_OK);
    }
    unsaved_files_invalidate();

    shared_resource_unlock(&m_resource);
}

void unsaved_file_patch(const char * p_filename, const char * p_new_contents, const position_t * p_start_pos, size_t old_len)
{
    shared_resource_lock(&m_resource);
    LOG("Patching unsaved file %s...\n", p_filename);
    source_file_t * p_file = NULL;
    ASSERT(hashtable_get(mp_unsaved_files, (void *) p_filename, &p_file) == CC_OK);
    LOG("Found unsaved file...\n");
    source_file_patch(p_file, p_new_contents, p_start_pos, old_len);
    unsaved_files_invalidate();
    shared_resource_unlock(&m_resource);
}

bool unsaved_file_remove(const char * p_filename)
{
    shared_resource_lock(&m_resource);
    LOG("Remove unsaved file %s\n", p_filename);
    source_file_t * p_file;
    bool success = (hashtable_remove(mp_unsaved_files, (void *) p_filename, &p_file) == CC_OK);
    if (success)
    {
        source_file_free(p_file);
        unsaved_files_invalidate();
    }
    shared_resource_unlock(&m_resource);
    return success;
}

void unsaved_files_release(void)
{
    shared_resource_release(&m_resource);
}