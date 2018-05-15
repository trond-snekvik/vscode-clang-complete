#include <stdint.h>
#include <clang-c/CXCompilationDatabase.h>
#include "unit_storage.h"
#include "hashtable.h"
#include "queue.h"
#include "path.h"
#include "log.h"
#include "json_rpc.h"
#include "unsaved_files.h"

typedef struct
{
    char * p_path;
    compile_flags_t flags;

} compile_directory_t;

typedef struct
{
    HashTable * p_table;
    HashTable * p_directories;
} unit_storage_t;

typedef struct
{
    Queue * p_queue;
    mutex_t mut;
    bool index_loaded;
    time_t prev_index_time;
} index_thread_context_t;

static compile_flags_t m_base_flags;
static unit_storage_t m_storage;
static bool m_exit;
static thread_t * mp_index_thread;
static thread_t * mp_reparse_thread;
static Queue * mp_change_queue;
static semaphore_t m_change_queue_sem;

static unit_diagnostics_callback_t m_diag_callback;

static void reparse_thread(void * p_context)
{
    while (!m_exit)
    {
        char * p_changed_file;
        if (queue_size(mp_change_queue) == 0)
        {
            semaphore_wait(&m_change_queue_sem);
        }

        bool found_elem = (queue_poll(mp_change_queue, &p_changed_file) == CC_OK);

        if (found_elem && hashtable_size(m_storage.p_table) > 0)
        {
            unsaved_files_t * p_unsaved_files = unsaved_files_get();
            LOG("ASYNC CHANGE: %s\n", p_changed_file);

            HashTableIter iter;
            hashtable_iter_init(&iter, m_storage.p_table);
            TableEntry * p_entry;
            while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
            {
                unit_t * p_unit = p_entry->value;
                if (unit_includes_file(p_unit, p_changed_file) && !path_equals(p_unit->p_filename, p_changed_file))
                {
                    unit_reparse(p_unit, p_unsaved_files->p_list, p_unsaved_files->count);
                    unit_diagnostics_get(p_unit, m_diag_callback, NULL, NULL);
                }
            }

            FREE(p_changed_file);
            unsaved_files_release(p_unsaved_files);
        }
    }
}

void unit_storage_init(const compile_flags_t * p_base_flags, unit_diagnostics_callback_t diag_callback)
{
    m_exit = false;
    ASSERT(hashtable_new(&m_storage.p_table) == CC_OK);
    ASSERT(hashtable_new(&m_storage.p_directories) == CC_OK);
    ASSERT(queue_new(&mp_change_queue) == CC_OK);
    semaphore_init(&m_change_queue_sem, 1);
    compile_flags_clone(&m_base_flags, p_base_flags);
    m_diag_callback = diag_callback;
    mp_reparse_thread = thread_start(reparse_thread, NULL, THREAD_PRIORITY_LOWEST);
    ASSERT(mp_reparse_thread);
}

void unit_storage_wait_for_completion(void)
{
    m_exit = true;
    if (mp_reparse_thread)
    {
        semaphore_signal(&m_change_queue_sem);
        thread_join(mp_reparse_thread);
    }
	if (mp_index_thread)
	{
		thread_join(mp_index_thread);
	}

    if (hashtable_size(m_storage.p_directories) > 0)
    {
        size_t match_len = 0;
        HashTableIter iter;
        hashtable_iter_init(&iter, m_storage.p_directories);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            compile_directory_t * p_it;
            ASSERT(hashtable_iter_remove(&iter, &p_it) == CC_OK);
            compile_flags_free(&p_it->flags);
            FREE(p_it->p_path);
            FREE(p_it);
        }
    }
    hashtable_destroy(m_storage.p_directories);

    if (hashtable_size(m_storage.p_table) > 0)
    {
        size_t match_len = 0;
        HashTableIter iter;
        hashtable_iter_init(&iter, m_storage.p_table);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            unit_t * p_it;
            ASSERT(hashtable_iter_remove(&iter, &p_it) == CC_OK);
            unit_free(p_it);
        }
    }
    hashtable_destroy(m_storage.p_table);
}

void unit_storage_notify_change(const char * p_filename)
{
    ASSERT(queue_enqueue(mp_change_queue, absolute_path(p_filename, path_cwd())) == CC_OK);
    semaphore_signal(&m_change_queue_sem);
}

static void index_thread(void * p_args)
{
    index_thread_context_t * p_context = p_args;

    while (true)
    {
        unit_t * p_unit;

        mutex_take(&p_context->mut);
        bool has_value = (queue_poll(p_context->p_queue, &p_unit) == CC_OK);
        mutex_release(&p_context->mut);

        if (has_value)
        {
            json_rpc_suspend();
            unsaved_files_t * p_unsaved_files = unsaved_files_get();

            time_t last_edit;
            bool found_file = path_last_edit(p_unit->p_filename, &last_edit);
            bool new_changes = (!p_context->index_loaded || last_edit >= p_context->prev_index_time);

            if (!p_unit->active && found_file && new_changes)
            {
                unit_index(p_unit, p_unsaved_files->p_list, p_unsaved_files->count);
            }
            unsaved_files_release(p_unsaved_files);
            json_rpc_resume();
        }
        else
        {
            break;
        }
    }
}

static void index_monitor_thread(void * p_args)
{
    index_thread_context_t * p_context = p_args;
    time_t index_start_time = time(0);

    profile_time_t start_time = profile_start();
    thread_t * p_threads[INDEXING_THREADS];

    p_context->index_loaded = false; //unit_index_load(&p_context->prev_index_time);
    if (p_context->index_loaded)
    {
        LOG("Loaded index\n");
    }

    for (unsigned i = 0; i < INDEXING_THREADS; ++i)
    {
        p_threads[i] = thread_start(index_thread, p_context, THREAD_PRIO_LOW);
    }

    for (unsigned i = 0; i < INDEXING_THREADS; ++i)
    {
        thread_join(p_threads[i]);
    }
    LOG("Indexing complete: %u ms\n", profile_end(start_time));

    mutex_free(&p_context->mut);
    queue_destroy(p_context->p_queue);
    unit_index_save(index_start_time);
    FREE(p_context);
}

bool unit_storage_compilation_database_load(const compilation_database_params_t * p_params)
{
    CXCompilationDatabase_Error status;
    CXCompilationDatabase db = clang_CompilationDatabase_fromDirectory(p_params->path, &status);
    if (status == CXCompilationDatabase_NoError)
    {
        CXCompileCommands commands = clang_CompilationDatabase_getAllCompileCommands(db);
        size_t command_count = clang_CompileCommands_getSize(commands);
        LOG("Found %u commands in %s\n", command_count, p_params->path);

        index_thread_context_t  * p_context = MALLOC(sizeof(index_thread_context_t));
        mutex_init(&p_context->mut);
        ASSERT(queue_new(&p_context->p_queue) == CC_OK);

        for (size_t i = 0; i < command_count; ++i)
        {
            CXCompileCommand command = clang_CompileCommands_getCommand(commands, i);

            CXString filename = clang_CompileCommand_getFilename(command);
            CXString directory = clang_CompileCommand_getDirectory(command);

            char * p_filename = normalize_path(clang_getCString(filename));

            unsigned command_flags_maxcount = clang_CompileCommand_getNumArgs(command);

            if (command_flags_maxcount && unit_storage_get(p_filename) == NULL)
            {
                compile_flags_t flags;
                flags.full_argv = true;
                flags.count = 0;
                flags.pp_array = MALLOC(sizeof(const char *) * (command_flags_maxcount + m_base_flags.count + p_params->additional_arguments_count));

                for (size_t j = 0; j < command_flags_maxcount; ++j)
                {
                    CXString arg = clang_CompileCommand_getArg(command, j);
                    const char * p_arg = clang_getCString(arg);
                    /* Make include paths absolute */
                    if (strstr(p_arg, "-I") == p_arg || strstr(p_arg, "-i") == p_arg)
                    {
                        char * p_absolute_path = absolute_path(&p_arg[2], clang_getCString(directory));
                        flags.pp_array[flags.count] = MALLOC(2 + strlen(p_absolute_path) + 1);
                        sprintf(flags.pp_array[flags.count], "-I%s", p_absolute_path);
                        FREE(p_absolute_path);
                        flags.count++;
                    }
                    else
                    {
                        flags.pp_array[flags.count++] = STRDUP(p_arg);
                    }
                    clang_disposeString(arg);
                }
                // add base flags after loaded flags
                for (size_t j = 0; j < m_base_flags.count; ++j)
                {
                    flags.pp_array[flags.count++] = STRDUP(m_base_flags.pp_array[j]);
                }
                // add user defined additional flags at the end
                for (size_t j = 0; j < p_params->additional_arguments_count; ++j)
                {
                    flags.pp_array[flags.count++] = STRDUP(p_params->p_additional_arguments[j]);
                }

                unit_t * p_unit = unit_create(p_filename, &flags);

                if (p_unit)
                {
                    queue_enqueue(p_context->p_queue, p_unit);
                    // unit_index(p_unit, p_unsaved_files->p_list, p_unsaved_files->count);
                    unit_storage_add(p_unit);
                }
                // remember directory
                char * p_directory = path_directory(p_filename);
                ASSERT(p_directory);
                compile_directory_t * p_dir;
                if (hashtable_get(m_storage.p_directories, p_directory, &p_dir) != CC_OK)
                {
                    p_dir = MALLOC(sizeof(compile_directory_t));
                    p_dir->p_path = p_directory;
                    compile_flags_clone(&p_dir->flags, &flags);
                    LOG("Added directory %s\n", p_directory);
                    ASSERT(hashtable_add(m_storage.p_directories, p_directory, p_dir) == CC_OK);
                }
                else
                {
                    FREE(p_directory);
                }

                compile_flags_free(&flags);
            }

            FREE(p_filename);
            clang_disposeString(filename);
            clang_disposeString(directory);
        }
        clang_CompileCommands_dispose(commands);
        //index_thread(p_context);
        mp_index_thread = thread_start(index_monitor_thread, p_context, THREAD_PRIO_NORMAL);
    }
    else
    {
        LOG("Failed loading DB: %u\n", status);
    }

    clang_CompilationDatabase_dispose(db);

    return (status == CXCompilationDatabase_NoError);
}

void unit_storage_add(unit_t * p_unit)
{
    ASSERT(unit_storage_get(p_unit->p_filename) == NULL);
    ASSERT(hashtable_add(m_storage.p_table, (void *) p_unit->p_filename, p_unit) == CC_OK);
}

unit_t * unit_storage_get(const char * p_filename)
{
    unit_t * p_unit = NULL;
    char * p_filename_normalized = normalize_path(p_filename);

    bool found = (hashtable_get(m_storage.p_table, (void *) p_filename_normalized, &p_unit) == CC_OK);

    if (!found)
    {
        const char * p_source_file = unit_index_source_for_header(p_filename_normalized);
        if (p_source_file)
        {
            found = (hashtable_get(m_storage.p_table, (void *) p_source_file, &p_unit) == CC_OK);
            if (found)
            {
                LOG("Returning %s for header %s\n", p_unit->p_filename, p_source_file);
            }
        }
    }

    FREE(p_filename_normalized);
    return (found ? p_unit : NULL);
}

unit_t * unit_storage_remove(const char * p_filename)
{
    unit_t * p_unit = NULL;
    if (hashtable_remove(m_storage.p_table, (void *) p_filename, &p_unit) == CC_OK)
    {
        return p_unit;
    }
    else
    {
        return NULL;
    }
}

bool unit_storage_flags_suggest(const char * p_filename, compile_flags_t * p_flags)
{
    if (hashtable_size(m_storage.p_directories) == 0)
    {
        return false;
    }
    char * p_dirname = path_directory(p_filename);

    // First look for a known folder:
    compile_directory_t * p_dir;
    if (hashtable_get(m_storage.p_directories, p_dirname, &p_dir) == CC_OK)
    {
        *p_flags = p_dir->flags;
    }
    else if (hashtable_size(m_storage.p_directories) > 0)
    {
        // find closest matching path:
        p_dir = NULL;
        size_t match_len = 0;
        HashTableIter iter;
        hashtable_iter_init(&iter, m_storage.p_directories);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            compile_directory_t * p_it = p_entry->value;
            char * p_common_parent = path_closest_common_ancestor(p_dirname, p_it->p_path);
            size_t length = strlen(p_common_parent);
            if (length > match_len)
            {
                match_len = length;
                p_dir = p_it;
            }
            FREE(p_common_parent);
        }

        if (p_dir)
        {
            *p_flags = p_dir->flags;
        }
    }
    FREE(p_dirname);

    return (p_dir != NULL);
}