#pragma once
#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
#include "structures.h"

static inline json_t * encode_message_type(message_type_t value)
{
    return json_integer(value);
}

static inline json_t * encode_file_change_type(file_change_type_t value)
{
    return json_integer(value);
}

static inline json_t * encode_watch_kind(watch_kind_t value)
{
    return json_integer(value);
}

static inline json_t * encode_text_document_sync_kind(text_document_sync_kind_t value)
{
    return json_integer(value);
}

static inline json_t * encode_diagnostic_severity(diagnostic_severity_t value)
{
    return json_integer(value);
}

static inline json_t * encode_completion_item_kind(completion_item_kind_t value)
{
    return json_integer(value);
}

static inline json_t * encode_insert_text_format(insert_text_format_t value)
{
    return json_integer(value);
}

static inline json_t * encode_document_highlight_kind(document_highlight_kind_t value)
{
    return json_integer(value);
}

static inline json_t * encode_symbol_kind(symbol_kind_t value)
{
    return json_integer(value);
}

static inline json_t * encode_text_document_save_reason(text_document_save_reason_t value)
{
    return json_integer(value);
}
/* Common parameter JSON encoders */
json_t * encode_initialization_options(initialization_options_t value);
json_t * encode_workspace_client_capabilities(workspace_client_capabilities_t value);
json_t * encode_text_document_client_capabilities(text_document_client_capabilities_t value);
json_t * encode_client_capabilities(client_capabilities_t value);
json_t * encode_completion_options(completion_options_t value);
json_t * encode_signature_help_options(signature_help_options_t value);
json_t * encode_code_lens_options(code_lens_options_t value);
json_t * encode_save_options(save_options_t value);
json_t * encode_text_document_sync_options(text_document_sync_options_t value);
json_t * encode_server_capabilities(server_capabilities_t value);
json_t * encode_markup_content(markup_content_t value);
json_t * encode_marked_string(marked_string_t value);
json_t * encode_message_action_item(message_action_item_t value);
json_t * encode_position(position_t value);
json_t * encode_range(range_t value);
json_t * encode_text_edit(text_edit_t value);
json_t * encode_location(location_t value);
json_t * encode_diagnostic_related_information(diagnostic_related_information_t value);
json_t * encode_diagnostic(diagnostic_t value);
json_t * encode_text_document_item(text_document_item_t value);
json_t * encode_text_document_content_change_event(text_document_content_change_event_t value);
json_t * encode_text_document_identifier(text_document_identifier_t value);
json_t * encode_versioned_text_document_identifier(versioned_text_document_identifier_t value);
json_t * encode_file_event(file_event_t value);
json_t * encode_text_document_position_params(text_document_position_params_t value);
json_t * encode_command(command_t value);
json_t * encode_completion_item(completion_item_t value);
json_t * encode_parameter_information(parameter_information_t value);
json_t * encode_signature_information(signature_information_t value);
json_t * encode_reference_context(reference_context_t value);
json_t * encode_completion_list(completion_list_t value);
json_t * encode_code_action_context(code_action_context_t value);
/* Server command parameter JSON encoders */
json_t * encode_initialize_params(initialize_params_t value);
json_t * encode_reference_params(reference_params_t value);
json_t * encode_document_symbol_params(document_symbol_params_t value);
json_t * encode_workspace_symbol_params(workspace_symbol_params_t value);
json_t * encode_document_link_params(document_link_params_t value);
json_t * encode_code_action_params(code_action_params_t value);
/* Client command parameter JSON encoders */
json_t * encode_show_message_request_params(show_message_request_params_t value);
json_t * encode_signature_help(signature_help_t value);
/* Command response parameter JSON encoders */
json_t * encode_initialize_result(initialize_result_t value);
json_t * encode_symbol_information(symbol_information_t value);
json_t * encode_document_link(document_link_t value);
json_t * encode_hover(hover_t value);
/* Client notification parameter JSON encoders */
json_t * encode_show_message_params(show_message_params_t value);
json_t * encode_log_message_params(log_message_params_t value);
json_t * encode_publish_diagnostics_params(publish_diagnostics_params_t value);
/* Server notification parameter JSON encoders */
json_t * encode_did_open_text_document_params(did_open_text_document_params_t value);
json_t * encode_did_change_text_document_params(did_change_text_document_params_t value);
json_t * encode_did_save_text_document_params(did_save_text_document_params_t value);
json_t * encode_did_close_text_document_params(did_close_text_document_params_t value);
json_t * encode_did_change_watched_files_params(did_change_watched_files_params_t value);
