#include <stdint.h>
#include <clang-c/CXCompilationDatabase.h>
#include "unit_storage.h"
#include "hashtable.h"
#include "path.h"
#include "log.h"
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



static unit_storage_t m_storage;

void unit_storage_init(void)
{
    ASSERT(hashtable_new(&m_storage.p_table) == CC_OK);
    ASSERT(hashtable_new(&m_storage.p_directories) == CC_OK);
}

bool unit_storage_compilation_database_load(const char * p_directory)
{
    CXCompilationDatabase_Error status;
    CXCompilationDatabase db = clang_CompilationDatabase_fromDirectory(p_directory, &status);
    if (status == CXCompilationDatabase_NoError)
    {
        CXCompileCommands commands = clang_CompilationDatabase_getAllCompileCommands(db);
        size_t command_count = clang_CompileCommands_getSize(commands);
        LOG("Found %u commands in %s\n", command_count, p_directory);
        for (size_t i = 0; i < command_count; ++i)
        {
            CXCompileCommand command = clang_CompileCommands_getCommand(commands, i);

            CXString filename = clang_CompileCommand_getFilename(command);
            CXString directory = clang_CompileCommand_getDirectory(command);

            char * p_filename = normalize_path(clang_getCString(filename));

            if (unit_storage_get(p_filename) == NULL)
            {
                compile_flags_t flags;
                flags.count = 0;
                flags.pp_array = NULL;
                unsigned max_count = clang_CompileCommand_getNumArgs(command);;

                if (max_count > 0)
                {
                    flags.pp_array = MALLOC(sizeof(const char *) * max_count);

                    bool skip_arg = false;
                    for (size_t j = 0; j < max_count; ++j)
                    {
                        CXString arg = clang_CompileCommand_getArg(command, j);
                        char * p_arg = STRDUP(clang_getCString(arg));

                        if ((p_arg[0] == '-' || j == 0) && !skip_arg)
                        {
                            skip_arg = false;
                            clang_disposeString(arg);
                            if (strcmp(p_arg, "-o") == 0)
                            {
                                skip_arg = true;
                            }
                            else
                            {
                                if (strstr(p_arg, "-I") == p_arg || strstr(p_arg, "-i") == p_arg && strstr(p_arg, "-isystem") != p_arg)
                                {
                                    char * p_abs_path = absolute_path(&p_arg[2], clang_getCString(directory));
                                    flags.pp_array[flags.count] = CALLOC(1, 2 + strlen(p_abs_path) + 1);
                                    sprintf(flags.pp_array[flags.count], "-I%s", p_abs_path);
                                    FREE(p_abs_path);
                                }
                                else
                                {
                                    flags.pp_array[flags.count] = STRDUP(p_arg);
                                }
                                flags.count++;
                            }
                        }

                        FREE(p_arg);
                    }
                }

                compile_flags_print(&flags);

                unit_t * p_unit = unit_create(p_filename, &flags);

                if (p_unit)
                {
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
        }
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
    else
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