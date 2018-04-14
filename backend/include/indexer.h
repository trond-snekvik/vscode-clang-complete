#pragma once
#include "structures.h"
#include "unit.h"
#include "hashtable.h"

typedef enum
{
    INDEX_SCOPE_LOCAL,
    INDEX_SCOPE_GLOBAL
} index_scope_t;

typedef struct
{
    const char * p_USR;
    char * p_name;
    location_t location;
    unit_t * p_unit;
    index_scope_t scope;
    symbol_kind_t kind;
} index_declaration_t;

typedef struct
{
    HashTable * p_table;
    mutex_t mut;
} index_t;


void index_init(index_t * p_index);

void index_declaration_add(index_t * p_index, const char * p_USR, index_declaration_t * p_decl);

Array * index_decls_get(index_t * p_index, const char * p_USR);
void index_decl_remove(index_t * p_index, const char * p_USR, index_declaration_t * p_decl);

void index_decl_free(index_declaration_t * p_decl);