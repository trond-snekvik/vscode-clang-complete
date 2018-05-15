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
    index_scope_t scope;
    symbol_kind_t kind;
} index_declaration_t;

typedef struct
{
    HashTable * p_table;
    HashTable * p_header_map;
    mutex_t mut;
} index_t;


void index_init(index_t * p_index);
void index_free(index_t * p_index);

void index_declaration_add(index_t * p_index, const char * p_USR, index_declaration_t * p_decl);
void index_header_set(index_t * p_index, const char * p_sourcefile, const char * p_headerfile);

Array * index_decls_get(index_t * p_index, const char * p_USR);

void index_decl_remove(index_t * p_index, const char * p_USR, index_declaration_t * p_decl);
void index_header_remove(index_t * p_index, const char * p_sourcefile);

const char * index_header_for_source(index_t * p_index, const char * p_sourcefile);
const char * index_source_for_header(index_t * p_index, const char * p_header);

void index_decl_free(index_declaration_t * p_decl);

bool index_save(index_t * p_index, const char * p_location, time_t timestamp);
bool index_load(index_t * p_index, const char * p_location, time_t * p_timestamp);