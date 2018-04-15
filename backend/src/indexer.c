#include "indexer.h"


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
