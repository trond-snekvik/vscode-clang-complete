#include "indexer.h"
#include "encoders.h"
#include "decoders.h"
#include "jansson.h"


typedef struct
{
    char * p_USR;
    Array * p_declarations;
} declaration_set_t;

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
    LOG("Removing %s\n", p_USR);
    declaration_set_t * p_set = get_set(p_index, p_USR);
    if (p_set)
    {
        array_remove(p_set->p_declarations, p_decl, NULL);
        p_decl->p_USR = NULL;
        if (array_size(p_set->p_declarations) == 0)
        {
            LOG("List for %s freed.\n", p_set->p_USR);
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

    bool success = (json_dump_file(p_root, p_location, 0) == 0);
    json_decref(p_root);
    return success;
}

bool index_load(index_t * p_index, const char * p_location, time_t * p_timestamp)
{
    json_error_t error;
    json_t * p_root = json_load_file(p_location, 0, &error);
    if (p_root)
    {
		*p_timestamp = json_integer_value(json_object_get(p_root, "timestamp"));

		json_t * p_declarations = json_object_get(p_root, "declarations");
        json_t * p_set;
        const char * p_USR;
        json_object_foreach(p_declarations, p_USR, p_set)
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
        json_decref(p_root);
    }
    return (p_root != NULL);
}
