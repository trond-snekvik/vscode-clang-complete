#pragma once
#include "hashtable.h"
#include "unit.h"




void unit_storage_init(const compile_flags_t * p_base_flags);
bool unit_storage_compilation_database_load(const char * p_directory);

void unit_storage_add(unit_t * p_unit);

unit_t * unit_storage_get(const char * p_filename);
unit_t * unit_storage_remove(const char * p_filename);
bool unit_storage_flags_suggest(const char * p_filename, compile_flags_t * p_flags);