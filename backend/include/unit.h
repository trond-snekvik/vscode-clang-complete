#ifndef UNIT_H__
#define UNIT_H__

#include <stdbool.h>
#include <stdint.h>
#include <clang-c/Index.h>
#include "lsp.h"
#include "hashtable.h"
#include "log.h"

#define DIAG_MAX_PER_FILE 100
#define DIAG_MAX_FILES 20

typedef struct
{
    char ** pp_array;
    unsigned count;
} compile_flags_t;

typedef struct
{
    char * p_string;
    char * p_filename;
    range_t range;
} fixit_t;

typedef struct
{
    char * p_filename;
    compile_flags_t flags;

    CXTranslationUnit tu;
    bool active;
    HashTable * diag_files;

    fixit_t * p_fixits;
    unsigned fixit_count;
    Array * p_declarations;
    Array * p_references;
} unit_t;

typedef struct
{
    uint32_t completion_priority_max;
    uint32_t completion_results_max;
    uint32_t diagnostics_max;
} unit_config_t;

typedef enum
{
    DEFINITION_TYPE_DEFINITION,
    DEFINITION_TYPE_REFERENCE,
} definition_type_t;

typedef void (*unit_completion_result_callback_t)(const completion_item_t * p_result, unsigned index, void * p_args);
typedef void (*unit_reference_callback_t)(const location_t * p_location, unsigned index, void * p_args);
typedef void (*unit_definition_callback_t)(const location_t * p_location, unsigned index, void * p_args, definition_type_t type);
typedef void (*unit_signature_callback_t)(const signature_information_t * p_signature, unsigned index, void * p_args);
typedef void (*unit_diagnostics_callback_t)(unit_t * p_unit, const publish_diagnostics_params_t * p_diagnostics, void * p_args);
typedef void (*unit_fixit_callback_t)(unit_t * p_unit, const publish_diagnostics_params_t * p_diagnostics, void * p_args);

void unit_init(const unit_config_t * p_config);

void unit_index(unit_t * p_unit,
                struct CXUnsavedFile * p_unsaved_files,
                uint32_t unsaved_file_count);

void unit_diagnostics_callback_set(unit_diagnostics_callback_t callback);

unit_t * unit_create(const char * p_filename,
                     const compile_flags_t * p_flags);


bool unit_parse(unit_t * p_unit,
                struct CXUnsavedFile * p_unsaved_files,
                uint32_t unsaved_file_count);

void unit_free(unit_t * p_unit);

void unit_suspend(unit_t * p_unit);

bool unit_reparse(unit_t * p_unit,
                  struct CXUnsavedFile * p_unsaved_files,
                  uint32_t unsaved_file_count);


bool unit_code_completion(unit_t * p_unit,
                          const text_document_position_params_t * p_position,
                          struct CXUnsavedFile * p_unsaved_files,
                          uint32_t unsaved_file_count,
                          unit_completion_result_callback_t callback,
                          void * p_args);

void unit_references_get(unit_t *p_unit, const reference_params_t *p_params,
                         unit_reference_callback_t callback, void *p_args);

unsigned unit_function_signature_get(unit_t *p_unit,
                                     const text_document_position_params_t *p_position,
                                     unit_signature_callback_t callback,
                                     void *p_args);

void unit_definition_get(unit_t *p_unit,
                         const text_document_position_params_t *p_position,
                         unit_definition_callback_t callback,
                         void *p_args);

bool unit_hover_get(unit_t * p_unit,
                    const text_document_position_params_t * p_position,
                    hover_t * p_hover);

unsigned unit_diagnostics_get(unit_t *p_unit,
                              unit_diagnostics_callback_t diag_callback,
                              unit_fixit_callback_t fixit_callback,
                              void *p_args);

void unit_fixits_resolve(unit_t * p_unit, const char * p_filename, const range_t * p_range, command_t * p_commands, unsigned * p_count);

bool unit_includes_file(unit_t * p_unit, const char * p_file);

static inline void compile_flags_clone(compile_flags_t * p_dst, const compile_flags_t * p_src)
{
    p_dst->count = p_src->count;
	if (p_src->count > 0)
	{
		p_dst->pp_array = malloc(sizeof(char *) * p_dst->count);
		for (unsigned i = 0; i < p_dst->count; ++i)
		{
			p_dst->pp_array[i] = STRDUP(p_src->pp_array[i]);
		}
	}
	else
	{
		p_dst->pp_array = NULL;
	}
}

static inline void compile_flags_free(compile_flags_t * p_flags)
{
    for (unsigned i = 0; i < p_flags->count; ++i)
    {
        free(p_flags->pp_array[i]);
    }
    free(p_flags->pp_array);
    p_flags->count = 0;
}

static inline void compile_flags_print(const compile_flags_t * p_flags)
{
    LOG("FLAGS: ");
    for (unsigned i = 0; i < p_flags->count; ++i)
    {
        LOG("%s ", p_flags->pp_array[i]);
    }
    LOG("\n");
}

#endif /* UNIT_H__ */