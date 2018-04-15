//#include "command_handler.h"

#include <clang-c/Index.h>

#include "lsp.h"
#include "message_handling.h"
#include "hashtable.h"
#include "log.h"
#include "source_file.h"
#include "unsaved_files.h"
#include "unit_storage.h"
#include "unit.h"
#include "utils.h"
#include "encoders.h"
#include "decoders.h"
#include "json_rpc.h"
#include "path.h"

#define REPARSE_RETRIES_MAX 5

static const char * m_base_flags[] = {"-ferror-limit=0"};

static char * m_signature_trigger_characters[] = {"(", ","};
static char * m_completion_trigger_characters[] = {".", ">", ":"};
static compile_flags_t m_flags;

static unit_t * mp_current_unit;

static void diag_callback(unit_t * p_unit, const publish_diagnostics_params_t * p_diagnostics, void * p_args)
{
    json_t * p_params = p_args;
    json_rpc_notification_send(LSP_NOTIFICATION_TEXT_DOCUMENT_PUBLISH_DIAGNOSTICS,
                               encode_publish_diagnostics_params(*p_diagnostics));
}

static void completion_callback(const completion_item_t * p_result, unsigned index, void * p_args)
{
    json_t * p_response = p_args;
    json_array_append_new(p_response, encode_completion_item(*p_result));
}

static void signature_callback(const signature_information_t * p_info, unsigned index, void * p_args)
{
    json_t * p_info_array = p_args;
    json_array_append_new(p_info_array, encode_signature_information(*p_info));
}

static void definition_callback(const location_t * p_location, unsigned index, void * p_args, definition_type_t type)
{
    ASSERT(p_args && p_location);
    json_t * p_definition_array = p_args;

    LOG("CALLBACK URI:\n\tscheme: %s\n\tauthority: %s\n\tpath: %s\n\tquery: %s\n\tfragment: %s\n\ttype: %s\n",
        p_location->uri.scheme,
        p_location->uri.authority,
        p_location->uri.path,
        p_location->uri.query,
        p_location->uri.fragment,
        (type == DEFINITION_TYPE_DEFINITION ? "DEFINITION" : "REFERENCE"));
    json_array_append_new(p_definition_array, encode_location(*p_location));
}

static void symbol_callback(const location_t * p_location, const char * p_name, symbol_kind_t kind, void * p_args)
{
    json_t * p_symbol_array = p_args;
    symbol_information_t info;
    info.kind = kind;
    info.location = *p_location;
    info.name = (char *) p_name;
    info.valid_fields = (SYMBOL_INFORMATION_FIELD_KIND |
                         SYMBOL_INFORMATION_FIELD_LOCATION |
                         SYMBOL_INFORMATION_FIELD_LOCATION |
                         SYMBOL_INFORMATION_FIELD_NAME);
    json_array_append_new(p_symbol_array, encode_symbol_information(info));
}

static unit_t * add_unit(const char * p_path)
{
    // Check if some better flags can be found
    compile_flags_t * p_flags = &m_flags;
    compile_flags_t flags;
    if (unit_storage_flags_suggest(p_path, &flags))
    {
        p_flags = &flags;
    }

    unit_t *p_unit = unit_create(p_path,
                                 p_flags);
    ASSERT(p_unit);
    unit_storage_add(p_unit);
    return p_unit;
}

static unit_t * get_or_create_unit(const char * p_filename)
{
    char * p_normalized_filename = normalize_path(p_filename);

    /* If this file is represented as a unit, reparse it. */
    unit_t * p_unit = unit_storage_get(p_normalized_filename);

    if (!p_unit)
    {
        p_unit = add_unit(p_normalized_filename);
    }
    FREE(p_normalized_filename);

    if (!p_unit->active)
    {
        const unsaved_files_t * p_unsaved_files = unsaved_files_get();
        if (!unit_parse(p_unit, p_unsaved_files->p_list, p_unsaved_files->count))
        {
            LOG("Failed parsing unit\n");
            compile_flags_print(&p_unit->flags);
            unit_storage_remove(p_unit->p_filename);
            unsaved_files_release();
            return NULL;
        }
        unsaved_files_release();
    }
    mp_current_unit = p_unit;
    return p_unit;
}
/******************************************************************************
 * Message Handlers
 *****************************************************************************/
static void handle_request_initialize(const initialize_params_t * p_params, json_t * p_response)
{
    if (p_params->valid_fields & INITIALIZE_PARAMS_FIELD_INITIALIZATION_OPTIONS)
    {
        if ((p_params->initialization_options.valid_fields & INITIALIZATION_OPTIONS_FIELD_FLAGS) && p_params->initialization_options.flags_count > 0)
        {
            m_flags.count = ARRAY_SIZE(m_base_flags) + p_params->initialization_options.flags_count;
            m_flags.pp_array = MALLOC(sizeof(char * ) * m_flags.count);

            for (unsigned i = 0; i < ARRAY_SIZE(m_base_flags); ++i)
            {
                m_flags.pp_array[i] = STRDUP(m_base_flags[i]);
            }

            for (unsigned i = 0; i < p_params->initialization_options.flags_count; ++i)
            {
                m_flags.pp_array[ARRAY_SIZE(m_base_flags) + i] =
                    STRDUP(p_params->initialization_options.p_flags[i]);
            }
            LOG("Got %u flags from extension.\n", m_flags.count);
        }

        if (p_params->initialization_options.valid_fields & INITIALIZATION_OPTIONS_FIELD_COMPILATION_DATABASE && p_params->initialization_options.compilation_database_count > 0)
        {
            for (unsigned i = 0; i < p_params->initialization_options.compilation_database_count; ++i)
            {
                bool loaded = unit_storage_compilation_database_load(p_params->initialization_options.p_compilation_database[i]);
                LOG("Loading of compilation database at %s %s.\n",
                    p_params->initialization_options.p_compilation_database[i],
                    loaded ? "was successful" : "FAILED");
            }
        }
    }

    initialize_result_t result =
    {
        .capabilities = {
            .text_document_sync =
            {
                .open_close = true,
                .change = TEXT_DOCUMENT_SYNC_KIND_FULL,
                .save =
                {
                    .include_text = false,
                    .valid_fields = SAVE_OPTIONS_FIELD_ALL
                },
                .valid_fields = TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_ALL
            },
            .completion_provider =
            {
                .resolve_provider = false,
                .p_trigger_characters = m_completion_trigger_characters,
                .trigger_characters_count = ARRAY_SIZE(m_completion_trigger_characters),
                .valid_fields = COMPLETION_OPTIONS_FIELD_ALL
            },
            .signature_help_provider =
            {
                .p_trigger_characters = m_signature_trigger_characters,
                .trigger_characters_count = ARRAY_SIZE(m_signature_trigger_characters),
                .valid_fields = SIGNATURE_HELP_OPTIONS_FIELD_ALL
            },
            .hover_provider = true,
            .definition_provider = true,
            .code_action_provider = true,
            .document_symbol_provider = true,
            .valid_fields = ( SERVER_CAPABILITIES_FIELD_TEXT_DOCUMENT_SYNC
                            | SERVER_CAPABILITIES_FIELD_COMPLETION_PROVIDER
                            | SERVER_CAPABILITIES_FIELD_SIGNATURE_HELP_PROVIDER
                            | SERVER_CAPABILITIES_FIELD_HOVER_PROVIDER
                            | SERVER_CAPABILITIES_FIELD_DEFINITION_PROVIDER
                            | SERVER_CAPABILITIES_FIELD_DOCUMENT_SYMBOL_PROVIDER
                            // | SERVER_CAPABILITIES_FIELD_CODE_ACTION_PROVIDER
                            )
        },
        .valid_fields = INITIALIZE_RESULT_FIELD_CAPABILITIES
    };
    json_rpc_response_send(p_response, encode_initialize_result(result));
}

static void handle_notification_text_document_did_save(const did_save_text_document_params_t * p_params)
{
    if (p_params->text_document.uri.path)
    {
        // unsaved_file_remove(p_params->text_document.uri.path);
    }
}

static void handle_notification_text_document_did_open(const did_open_text_document_params_t * p_params)
{
    if (p_params->text_document.uri.path)
    {
        unsaved_file_set(p_params->text_document.uri.path, p_params->text_document.text);

        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);

        if (p_unit)
        {
            unit_diagnostics_get(p_unit, diag_callback, NULL, NULL);
        }

    }
}

static void handle_notification_text_document_did_change(const did_change_text_document_params_t * p_params)
{
    LOG("Handle did change\n");
    if (p_params->text_document.uri.path)
    {
        for (unsigned i = 0; i < p_params->content_changes_count; ++i)
        {
            LOG("Handle change #%u of %u\n", i, p_params->content_changes_count);
            if (p_params->p_content_changes[i].valid_fields == TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_ALL)
            {
                unsaved_file_patch(p_params->text_document.uri.path,
                                   p_params->p_content_changes[i].text,
                                   &p_params->p_content_changes[i].range.start,
                                   (size_t) p_params->p_content_changes[i].range_length);
            }
            else
            {
                ASSERT(p_params->p_content_changes[i].valid_fields == TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_TEXT);
                unsaved_file_set(p_params->text_document.uri.path, p_params->p_content_changes[i].text);
            }
        }

        LOG("Fetching unit %s (0x%p)\n", p_params->text_document.uri.path, p_params->text_document.uri.path);

        /* If this file is represented as a unit, reparse it. */
        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);
        if (p_unit)
        {
            LOG("\tFound file.\n");

            const unsaved_files_t * p_unsaved_files = unsaved_files_get();
            unsigned retries = 0;
            while (!unit_reparse(p_unit, p_unsaved_files->p_list, p_unsaved_files->count) && retries < REPARSE_RETRIES_MAX)
            {
                retries++;
            }
            unsaved_files_release();

            if (retries == REPARSE_RETRIES_MAX)
            {
                LOG("Reparse of %s failed. Removing unit.\n", p_unit->p_filename);
                unit_storage_remove(p_unit->p_filename);
                unit_free(p_unit);
                p_unit = NULL;
                mp_current_unit = NULL;
            }
            else
            {
                LOG("Reparse of %s successful\n", p_params->text_document.uri.path);
                unit_diagnostics_get(p_unit, diag_callback, NULL, NULL);
            }
        }
    }
}

static void handle_notification_text_document_did_close(const did_close_text_document_params_t * p_params)
{
    if (p_params->text_document.uri.path)
    {
        unit_t * p_unit = unit_storage_remove(p_params->text_document.uri.path);
        if (p_unit)
        {
            unit_free(p_unit);
            mp_current_unit = NULL;
        }
    }
}

static void handle_request_text_document_completion(const text_document_position_params_t * p_params, json_t * p_response)
{
    if (p_params->text_document.uri.path)
    {
        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);

        if (p_unit)
        {
            LOG("Completion at %s:%llu:%llu\n", p_params->text_document.uri.path, p_params->position.line, p_params->position.character);
            json_t * p_response_params = json_array();
            const unsaved_files_t * p_unsaved_files = unsaved_files_get();
            bool complete = unit_code_completion(p_unit, p_params, p_unsaved_files->p_list, p_unsaved_files->count, completion_callback, p_response_params);
            unsaved_files_release();

            LOG("Code completion successful (%u results%s)\n", json_array_size(p_response_params), complete ? "" : ", incomplete");
            if (complete)
            {
                json_rpc_response_send(p_response, p_response_params);
            }
            else
            {
                json_t * p_response_obj = json_object();
                json_object_set_new(p_response_obj, "isIncomplete", json_true());
                json_object_set_new(p_response_obj, "items", p_response_params);
                json_rpc_response_send(p_response, p_response_obj);
            }
        }
        else
        {
            json_rpc_error_response_send(p_response, 1, "No unit found", NULL);
        }
    }
    else
    {
        json_rpc_error_response_send(p_response, 2, "URI doesn't have a path components", NULL);
    }
}

static void handle_request_text_document_signature_help(const text_document_position_params_t * p_params, json_t * p_response)
{
    if (p_params->text_document.uri.path)
    {
        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);
        if (p_unit)
        {
            json_t * p_signature_array = json_array();
            unsigned param_index = unit_function_signature_get(p_unit, p_params, signature_callback, p_signature_array);
            json_t * p_response_params = json_object();
            json_object_set_new(p_response_params, "signatures", p_signature_array);
            json_object_set_new(p_response_params, "activeSignature", json_integer(0));
            json_object_set_new(p_response_params, "activeParameter", json_integer(param_index));
            json_rpc_response_send(p_response, p_response_params);
        }
        else
        {
            json_rpc_error_response_send(p_response, 1, "No unit found", NULL);
        }
    }
    else
    {
        json_rpc_error_response_send(p_response, 1, "Not a file with a path", NULL);
    }
}

static void handle_request_text_document_definition(const text_document_position_params_t * p_params, json_t * p_response)
{
    if (p_params->text_document.uri.path)
    {
        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);
        if (p_unit)
        {
            json_t * p_location_array = json_array();
            unit_definition_get(p_unit, p_params, definition_callback, p_location_array);
            LOG("Sending response...\n");
            json_rpc_response_send(p_response, p_location_array);
        }
        else
        {
            json_rpc_error_response_send(p_response, 1, "No unit found", NULL);
        }
    }
    else
    {
        json_rpc_error_response_send(p_response, 1, "Not a file with a path", NULL);
    }
}

static void handle_request_text_document_hover(const text_document_position_params_t * p_params, json_t * p_response)
{
    hover_t hover;

    if (p_params->text_document.uri.path)
    {
        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);
        if (p_unit)
        {
            if (unit_hover_get(p_unit, p_params, &hover))
            {
                json_rpc_response_send(p_response, encode_hover(hover));
                free_hover(hover);
            }
            else
            {
                json_rpc_response_send(p_response, json_null());
            }
        }
        else
        {
            json_rpc_error_response_send(p_response, 1, "No unit found", NULL);
        }
    }
    else
    {
        json_rpc_error_response_send(p_response, 1, "Not a file with a path", NULL);
    }
}

static void handle_request_text_document_code_action(const code_action_params_t * p_params, json_t * p_response)
{
    json_t * p_rsp_args = json_array();
    if (mp_current_unit)
    {
        unsigned count = mp_current_unit->fixit_count;
        command_t * p_commands = malloc(count * sizeof(command_t));
        unit_fixits_resolve(mp_current_unit, p_params->text_document.uri.path, &p_params->range, p_commands, &count);
        for (unsigned i = 0; i < count; ++i)
        {
            json_array_append_new(p_rsp_args, encode_command(p_commands[i]));
            free(p_commands[i].title);
        }
        free(p_commands);
    }

    json_rpc_response_send(p_response, p_rsp_args);
}

static void handle_request_text_document_document_symbol(const document_symbol_params_t * p_params, json_t * p_response)
{
    if (p_params->text_document.uri.path)
    {
        unit_t * p_unit = get_or_create_unit(p_params->text_document.uri.path);
        if (p_unit)
        {
            json_t * p_symbol_array = json_array();
            unit_symbols_get(p_unit, symbol_callback, p_symbol_array);
            json_rpc_response_send(p_response, p_symbol_array);
        }
        else
        {
            json_rpc_error_response_send(p_response, 1, "No unit found", NULL);
        }
    }
    else
    {
        json_rpc_error_response_send(p_response, 1, "Not a file with a path", NULL);
    }
}

void command_handler_init(void)
{
    unit_config_t config;
    config.completion_priority_max = 10000;
    config.completion_results_max = 500;
    config.diagnostics_max = 1000;
    unit_init(&config);
    const compile_flags_t base_flags = {
        .pp_array = (char **) m_base_flags,
        .count = ARRAY_SIZE(m_base_flags)
    };
    unit_storage_init(&base_flags);
    unsaved_files_init();

    lsp_request_handler_initialize_register(handle_request_initialize);
    lsp_notification_handler_text_document_did_save_register(handle_notification_text_document_did_save);
    lsp_notification_handler_text_document_did_change_register(handle_notification_text_document_did_change);
    lsp_notification_handler_text_document_did_open_register(handle_notification_text_document_did_open);
    lsp_notification_handler_text_document_did_close_register(handle_notification_text_document_did_close);
    lsp_request_handler_text_document_completion_register(handle_request_text_document_completion);
    lsp_request_handler_text_document_signature_help_register(handle_request_text_document_signature_help);
    lsp_request_handler_text_document_definition_register(handle_request_text_document_definition);
    lsp_request_handler_text_document_hover_register(handle_request_text_document_hover);
    lsp_request_handler_text_document_code_action_register(handle_request_text_document_code_action);
    lsp_request_handler_text_document_document_symbol_register(handle_request_text_document_document_symbol);
}

