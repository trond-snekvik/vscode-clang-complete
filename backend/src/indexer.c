#include "indexer.h"
#include "encoders.h"
#include "decoders.h"
#include "path.h"
#include "jansson.h"


typedef struct
{
    char * p_USR;
    Array * p_declarations;
} declaration_set_t;

typedef struct
{
    char * p_sourcefile;
    char * p_headerfile;
} header_map_entry_t;

static declaration_set_t * get_set(index_t * p_index, const char * p_USR)
{
    declaration_set_t * p_set;
    if (hashtable_get(p_index->p_table, (char *) p_USR, &p_set) == CC_OK)
    {
        return p_set;
    }
    return NULL;
}

void index_init(index_t * p_index)
{
    mutex_init(&p_index->mut);
    ASSERT(hashtable_new(&p_index->p_table) == CC_OK);
    ASSERT(hashtable_new(&p_index->p_header_map) == CC_OK);
}

void index_free(index_t * p_index)
{

    if (hashtable_size(p_index->p_table) > 0)
    {
        json_t * p_declarations = json_object();
        HashTableIter iter;
        hashtable_iter_init(&iter, p_index->p_table);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            declaration_set_t * p_decl_set;
            hashtable_iter_remove(&iter, &p_decl_set);
            ArrayIter decl_iter;
            array_iter_init(&decl_iter, p_decl_set->p_declarations);
            index_declaration_t * p_decl;
            while (array_iter_next(&decl_iter, &p_decl) == CC_OK)
            {
                index_decl_free(p_decl);
            }

            array_destroy(p_decl_set->p_declarations);
            FREE(p_decl_set->p_USR);
            FREE(p_decl_set);
        }
    }
    hashtable_destroy(p_index->p_table);
}

void index_declaration_add(index_t * p_index, const char * p_USR, index_declaration_t * p_decl)
{
    mutex_take(&p_index->mut);
    declaration_set_t * p_set;
    if (hashtable_get(p_index->p_table, (char *) p_USR, &p_set) != CC_OK)
    {
        p_set = MALLOC(sizeof(declaration_set_t));
        ASSERT(p_set);
        p_set->p_USR = STRDUP(p_USR);
        ASSERT(array_new(&p_set->p_declarations) == CC_OK);

        ASSERT(hashtable_add(p_index->p_table, p_set->p_USR, p_set) == CC_OK);
    }
    p_decl->p_USR = p_set->p_USR;
    ASSERT(array_add(p_set->p_declarations, p_decl) == CC_OK);

    mutex_release(&p_index->mut);
}

void index_header_set(index_t * p_index, const char * p_sourcefile, const char * p_headerfile)
{
    mutex_take(&p_index->mut);
    if (p_sourcefile && p_headerfile)
    {
        header_map_entry_t * p_entry;
        if (hashtable_get(p_index->p_header_map, (void *)p_sourcefile, &p_entry) == CC_OK)
        {
            FREE(p_entry->p_headerfile);
            p_entry->p_headerfile = STRDUP(p_headerfile);
        }
        else
        {
            p_entry = MALLOC(sizeof(header_map_entry_t));
            p_entry->p_sourcefile = STRDUP(p_sourcefile);
            p_entry->p_headerfile = STRDUP(p_headerfile);
            ASSERT(hashtable_add(p_index->p_header_map, p_entry->p_sourcefile, p_entry) == CC_OK);
        }
    }
    mutex_release(&p_index->mut);
}

Array * index_decls_get(index_t * p_index, const char * p_USR)
{
    declaration_set_t * p_set = get_set(p_index, p_USR);
    if (p_set)
    {
        return p_set->p_declarations;
    }
    return NULL;
}

void index_decl_remove(index_t * p_index, const char * p_USR, index_declaration_t * p_decl)
{
    mutex_take(&p_index->mut);
    declaration_set_t * p_set = get_set(p_index, p_USR);
    if (p_set)
    {
        array_remove(p_set->p_declarations, p_decl, NULL);
        p_decl->p_USR = NULL;
        if (array_size(p_set->p_declarations) == 0)
        {
            hashtable_remove(p_index->p_table, p_set->p_USR, NULL);
            array_destroy(p_set->p_declarations);
            FREE(p_set->p_USR);
            FREE(p_set);
        }
    }
    else
    {
        LOG("Removing %s failed!\n", p_USR);
    }
    mutex_release(&p_index->mut);
}

void index_header_remove(index_t * p_index, const char * p_sourcefile)
{
    mutex_take(&p_index->mut);
    header_map_entry_t * p_entry;
    if (hashtable_remove(p_index->p_header_map, (void *) p_sourcefile, &p_entry) == CC_OK)
    {
        FREE(p_entry->p_sourcefile);
        FREE(p_entry->p_headerfile);
        FREE(p_entry);
    }
    else
    {
        LOG("Removing header entry for %s failed.\n", p_sourcefile);
    }
    mutex_release(&p_index->mut);
}

void index_decl_free(index_declaration_t * p_decl)
{
    uri_free_members(&p_decl->location.uri);
    FREE(p_decl->p_name);
    FREE(p_decl);
}

bool index_save(index_t * p_index, const char * p_location, time_t timestamp)
{
    json_t * p_root = json_object();
	json_object_set_new(p_root, "timestamp", json_integer(timestamp));

    if (hashtable_size(p_index->p_table) > 0)
    {
	    json_t * p_declarations = json_object();
        HashTableIter iter;
        hashtable_iter_init(&iter, p_index->p_table);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            declaration_set_t * p_decl_set = p_entry->value;
            json_t * p_decl_array = json_array();

            ArrayIter decl_iter;
            array_iter_init(&decl_iter, p_decl_set->p_declarations);
            index_declaration_t * p_decl;
            while (array_iter_next(&decl_iter, &p_decl) == CC_OK)
            {
                json_t * p_decl_obj = json_object();
                json_object_set_new(p_decl_obj, "name", json_string(p_decl->p_name));
                json_object_set_new(p_decl_obj, "location", encode_location(p_decl->location));
                json_object_set_new(p_decl_obj, "scope", json_integer(p_decl->scope));
                json_object_set_new(p_decl_obj, "kind", json_integer(p_decl->kind));
                json_array_append_new(p_decl_array, p_decl_obj);
            }

            json_object_set_new(p_declarations, p_decl_set->p_USR, p_decl_array);
        }
        json_object_set_new(p_root, "declarations", p_declarations);
    }

    if (hashtable_size(p_index->p_header_map) > 0)
    {
        json_t * p_headermap = json_object();
        HashTableIter iter;
        hashtable_iter_init(&iter, p_index->p_header_map);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            header_map_entry_t * p_header_entry = p_entry->value;
            json_object_set_new(p_headermap, p_header_entry->p_sourcefile, json_string(p_header_entry->p_headerfile));
        }
        json_object_set_new(p_root, "headers", p_headermap);
    }

    bool success = (json_dump_file(p_root, p_location, 0) == 0);
    json_decref(p_root);
    return success;
}

bool index_load(index_t * p_index, const char * p_location, time_t * p_timestamp)
{
    json_error_t error;
    json_t * p_root = json_load_file(p_location, 0, &error);
    bool success = false;
    if (p_root)
    {
        json_t * p_json_timestamp = json_object_get(p_root, "timestamp");
        json_t * p_json_declarations = json_object_get(p_root, "declarations");
        json_t * p_json_headers = json_object_get(p_root, "headers");

        if (p_json_timestamp && p_json_declarations && p_json_headers)
        {
            success = true;
            *p_timestamp = json_integer_value(p_json_timestamp);

            json_t * p_set;
            const char * p_USR;
            json_object_foreach(p_json_declarations, p_USR, p_set)
            {
                json_t * p_decl_obj;
                unsigned i;
                json_array_foreach(p_set, i, p_decl_obj)
                {
                    index_declaration_t * p_decl = MALLOC(sizeof(index_declaration_t));
                    p_decl->p_name = STRDUP(json_string_value(json_object_get(p_decl_obj, "name")));
                    p_decl->location = decode_location(json_object_get(p_decl_obj, "location"));
                    p_decl->scope = (index_scope_t) json_integer_value(json_object_get(p_decl_obj, "scope"));
                    p_decl->kind = (symbol_kind_t) json_integer_value(json_object_get(p_decl_obj, "kind"));
                    index_declaration_add(p_index, p_USR, p_decl);
                }
            }

            json_t * p_header;
            const char * p_source;
            json_object_foreach(p_json_headers, p_source, p_header)
            {
                index_header_set(p_index, p_source, json_string_value(p_header));
            }
        }
        json_decref(p_root);
    }

    if (!success)
    {
        /* Touch the file to wipe it. */
        FILE * f = fopen(p_location, "w");
        fclose(f);
    }
    return success;
}


const char * index_header_for_source(index_t * p_index, const char * p_sourcefile)
{
    header_map_entry_t * p_entry;
    if (hashtable_get(p_index->p_header_map, (void *) p_sourcefile, &p_entry) == CC_OK)
    {
        return p_entry->p_sourcefile;
    }
    else
    {
        return NULL;
    }
}

const char * index_source_for_header(index_t * p_index, const char * p_header)
{
    if (hashtable_size(p_index->p_header_map) > 0)
    {
        HashTableIter iter;
        hashtable_iter_init(&iter, p_index->p_header_map);
        TableEntry * p_entry;
        while (hashtable_iter_next(&iter, &p_entry) == CC_OK)
        {
            header_map_entry_t * p_header_entry = p_entry->value;
            if (path_equals(p_header, p_header_entry->p_headerfile))
            {
                return p_header_entry->p_sourcefile;
            }
        }
    }
    return NULL;
}
