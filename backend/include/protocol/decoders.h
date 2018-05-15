#pragma once
#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
#include "structures.h"

typedef enum
{
    DECODER_ERROR_NONE,
    DECODER_ERROR_MISSING_REQUIRED_FIELDS
} decoder_error_t;

decoder_error_t decoder_error(void);

/*******************************************************************************
 * Decoders
 ******************************************************************************/
static inline message_type_t decode_message_type(json_t * p_json)
{
    return (message_type_t) decode_number(p_json);
}

static inline file_change_type_t decode_file_change_type(json_t * p_json)
{
    return (file_change_type_t) decode_number(p_json);
}

static inline watch_kind_t decode_watch_kind(json_t * p_json)
{
    return (watch_kind_t) decode_number(p_json);
}

static inline text_document_sync_kind_t decode_text_document_sync_kind(json_t * p_json)
{
    return (text_document_sync_kind_t) decode_number(p_json);
}

static inline diagnostic_severity_t decode_diagnostic_severity(json_t * p_json)
{
    return (diagnostic_severity_t) decode_number(p_json);
}

static inline completion_item_kind_t decode_completion_item_kind(json_t * p_json)
{
    return (completion_item_kind_t) decode_number(p_json);
}

static inline insert_text_format_t decode_insert_text_format(json_t * p_json)
{
    return (insert_text_format_t) decode_number(p_json);
}

static inline document_highlight_kind_t decode_document_highlight_kind(json_t * p_json)
{
    return (document_highlight_kind_t) decode_number(p_json);
}

static inline symbol_kind_t decode_symbol_kind(json_t * p_json)
{
    return (symbol_kind_t) decode_number(p_json);
}

static inline text_document_save_reason_t decode_text_document_save_reason(json_t * p_json)
{
    return (text_document_save_reason_t) decode_number(p_json);
}
/* Common parameter JSON decoders */
compilation_database_params_t decode_compilation_database_params(json_t * p_json);
initialization_options_t decode_initialization_options(json_t * p_json);
workspace_client_capabilities_t decode_workspace_client_capabilities(json_t * p_json);
text_document_client_capabilities_t decode_text_document_client_capabilities(json_t * p_json);
client_capabilities_t decode_client_capabilities(json_t * p_json);
completion_options_t decode_completion_options(json_t * p_json);
signature_help_options_t decode_signature_help_options(json_t * p_json);
code_lens_options_t decode_code_lens_options(json_t * p_json);
save_options_t decode_save_options(json_t * p_json);
text_document_sync_options_t decode_text_document_sync_options(json_t * p_json);
server_capabilities_t decode_server_capabilities(json_t * p_json);
markup_content_t decode_markup_content(json_t * p_json);
marked_string_t decode_marked_string(json_t * p_json);
message_action_item_t decode_message_action_item(json_t * p_json);
position_t decode_position(json_t * p_json);
range_t decode_range(json_t * p_json);
text_edit_t decode_text_edit(json_t * p_json);
location_t decode_location(json_t * p_json);
diagnostic_related_information_t decode_diagnostic_related_information(json_t * p_json);
diagnostic_t decode_diagnostic(json_t * p_json);
text_document_item_t decode_text_document_item(json_t * p_json);
text_document_content_change_event_t decode_text_document_content_change_event(json_t * p_json);
text_document_identifier_t decode_text_document_identifier(json_t * p_json);
versioned_text_document_identifier_t decode_versioned_text_document_identifier(json_t * p_json);
file_event_t decode_file_event(json_t * p_json);
text_document_position_params_t decode_text_document_position_params(json_t * p_json);
command_t decode_command(json_t * p_json);
completion_item_t decode_completion_item(json_t * p_json);
parameter_information_t decode_parameter_information(json_t * p_json);
signature_information_t decode_signature_information(json_t * p_json);
reference_context_t decode_reference_context(json_t * p_json);
completion_list_t decode_completion_list(json_t * p_json);
code_action_context_t decode_code_action_context(json_t * p_json);
/* Server command parameter JSON decoders */
initialize_params_t decode_initialize_params(json_t * p_json);
reference_params_t decode_reference_params(json_t * p_json);
document_symbol_params_t decode_document_symbol_params(json_t * p_json);
workspace_symbol_params_t decode_workspace_symbol_params(json_t * p_json);
document_link_params_t decode_document_link_params(json_t * p_json);
code_action_params_t decode_code_action_params(json_t * p_json);
/* Client command parameter JSON decoders */
show_message_request_params_t decode_show_message_request_params(json_t * p_json);
signature_help_t decode_signature_help(json_t * p_json);
/* Command response parameter JSON decoders */
initialize_result_t decode_initialize_result(json_t * p_json);
symbol_information_t decode_symbol_information(json_t * p_json);
document_link_t decode_document_link(json_t * p_json);
hover_t decode_hover(json_t * p_json);
/* Client notification parameter JSON decoders */
show_message_params_t decode_show_message_params(json_t * p_json);
log_message_params_t decode_log_message_params(json_t * p_json);
publish_diagnostics_params_t decode_publish_diagnostics_params(json_t * p_json);
/* Server notification parameter JSON decoders */
did_open_text_document_params_t decode_did_open_text_document_params(json_t * p_json);
did_change_text_document_params_t decode_did_change_text_document_params(json_t * p_json);
did_save_text_document_params_t decode_did_save_text_document_params(json_t * p_json);
did_close_text_document_params_t decode_did_close_text_document_params(json_t * p_json);
did_change_watched_files_params_t decode_did_change_watched_files_params(json_t * p_json);

/*******************************************************************************
 * Freers
 ******************************************************************************/
/* Common parameter structure freers */
void free_compilation_database_params(compilation_database_params_t value);
void free_initialization_options(initialization_options_t value);
void free_workspace_client_capabilities(workspace_client_capabilities_t value);
void free_text_document_client_capabilities(text_document_client_capabilities_t value);
void free_client_capabilities(client_capabilities_t value);
void free_completion_options(completion_options_t value);
void free_signature_help_options(signature_help_options_t value);
void free_code_lens_options(code_lens_options_t value);
void free_save_options(save_options_t value);
void free_text_document_sync_options(text_document_sync_options_t value);
void free_server_capabilities(server_capabilities_t value);
void free_markup_content(markup_content_t value);
void free_marked_string(marked_string_t value);
void free_message_action_item(message_action_item_t value);
void free_position(position_t value);
void free_range(range_t value);
void free_text_edit(text_edit_t value);
void free_location(location_t value);
void free_diagnostic_related_information(diagnostic_related_information_t value);
void free_diagnostic(diagnostic_t value);
void free_text_document_item(text_document_item_t value);
void free_text_document_content_change_event(text_document_content_change_event_t value);
void free_text_document_identifier(text_document_identifier_t value);
void free_versioned_text_document_identifier(versioned_text_document_identifier_t value);
void free_file_event(file_event_t value);
void free_text_document_position_params(text_document_position_params_t value);
void free_command(command_t value);
void free_completion_item(completion_item_t value);
void free_parameter_information(parameter_information_t value);
void free_signature_information(signature_information_t value);
void free_reference_context(reference_context_t value);
void free_completion_list(completion_list_t value);
void free_code_action_context(code_action_context_t value);
/* Server command parameter structure freers */
void free_initialize_params(initialize_params_t value);
void free_reference_params(reference_params_t value);
void free_document_symbol_params(document_symbol_params_t value);
void free_workspace_symbol_params(workspace_symbol_params_t value);
void free_document_link_params(document_link_params_t value);
void free_code_action_params(code_action_params_t value);
/* Client command parameter structure freers */
void free_show_message_request_params(show_message_request_params_t value);
void free_signature_help(signature_help_t value);
/* Command response parameter structure freers */
void free_initialize_result(initialize_result_t value);
void free_symbol_information(symbol_information_t value);
void free_document_link(document_link_t value);
void free_hover(hover_t value);
/* Client notification parameter structure freers */
void free_show_message_params(show_message_params_t value);
void free_log_message_params(log_message_params_t value);
void free_publish_diagnostics_params(publish_diagnostics_params_t value);
/* Server notification parameter structure freers */
void free_did_open_text_document_params(did_open_text_document_params_t value);
void free_did_change_text_document_params(did_change_text_document_params_t value);
void free_did_save_text_document_params(did_save_text_document_params_t value);
void free_did_close_text_document_params(did_close_text_document_params_t value);
void free_did_change_watched_files_params(did_change_watched_files_params_t value);
