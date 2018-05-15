#pragma once
#include "hashtable.h"
#include "unit.h"
#include "structures.h"



void unit_storage_init(const compile_flags_t * p_base_flags, unit_diagnostics_callback_t diag_callback);
void unit_storage_wait_for_completion(void);
void unit_storage_notify_change(const char * p_filename);
bool unit_storage_compilation_database_load(const compilation_database_params_t * p_params);

void unit_storage_add(unit_t * p_unit);

unit_t * unit_storage_get(const char * p_filename);
unit_t * unit_storage_remove(const char * p_filename);
bool unit_storage_flags_suggest(const char * p_filename, compile_flags_t * p_flags);