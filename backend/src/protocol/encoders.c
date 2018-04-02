#include "encoders.h"
#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
#include "structures.h"

/* Common parameter JSON encoders */
json_t * encode_initialization_options(initialization_options_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & INITIALIZATION_OPTIONS_FIELD_FLAGS)
    {
        json_t * p_flags_json = json_array();
        for (uint32_t i = 0; i < value.flags_count; ++i)
        {
            json_array_append_new(p_flags_json, encode_string(value.p_flags[i]));
        }
        json_object_set_new(p_json, "flags", p_flags_json);
    }

    if (value.valid_fields & INITIALIZATION_OPTIONS_FIELD_COMPILATION_DATABASE)
    {
        json_t * p_compilation_database_json = json_array();
        for (uint32_t i = 0; i < value.compilation_database_count; ++i)
        {
            json_array_append_new(p_compilation_database_json, encode_string(value.p_compilation_database[i]));
        }
        json_object_set_new(p_json, "compilationDatabase", p_compilation_database_json);
    }
    return p_json;
}

json_t * encode_workspace_client_capabilities(workspace_client_capabilities_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & WORKSPACE_CLIENT_CAPABILITIES_FIELD_APPLY_EDIT)
    {
        json_object_set_new(p_json, "applyEdit", encode_boolean(value.apply_edit));
    }

    if (value.valid_fields & WORKSPACE_CLIENT_CAPABILITIES_FIELD_WORKSPACE_EDIT)
    {
        json_t * p_workspace_edit_json = json_object();
        json_object_set_new(p_workspace_edit_json, "dynamicRegistration", encode_boolean(value.workspace_edit.dynamic_registration));
        json_object_set_new(p_json, "workspaceEdit", p_workspace_edit_json);
    }

    if (value.valid_fields & WORKSPACE_CLIENT_CAPABILITIES_FIELD_DID_CHANGE_CONFIGURATION)
    {
        json_t * p_did_change_configuration_json = json_object();
        json_object_set_new(p_did_change_configuration_json, "dynamicRegistration", encode_boolean(value.did_change_configuration.dynamic_registration));
        json_object_set_new(p_json, "didChangeConfiguration", p_did_change_configuration_json);
    }

    if (value.valid_fields & WORKSPACE_CLIENT_CAPABILITIES_FIELD_DID_CHANGE_WATCHED_FILES)
    {
        json_t * p_did_change_watched_files_json = json_object();
        json_object_set_new(p_did_change_watched_files_json, "dynamicRegistration", encode_boolean(value.did_change_watched_files.dynamic_registration));
        json_object_set_new(p_json, "didChangeWatchedFiles", p_did_change_watched_files_json);
    }

    if (value.valid_fields & WORKSPACE_CLIENT_CAPABILITIES_FIELD_SYMBOL)
    {
        json_t * p_symbol_json = json_object();
        json_object_set_new(p_symbol_json, "dynamicRegistration", encode_boolean(value.symbol.dynamic_registration));
        json_object_set_new(p_json, "symbol", p_symbol_json);
    }

    if (value.valid_fields & WORKSPACE_CLIENT_CAPABILITIES_FIELD_EXECUTE_COMMAND)
    {
        json_t * p_execute_command_json = json_object();
        json_object_set_new(p_execute_command_json, "dynamicRegistration", encode_boolean(value.execute_command.dynamic_registration));
        json_object_set_new(p_json, "executeCommand", p_execute_command_json);
    }
    return p_json;
}

json_t * encode_text_document_client_capabilities(text_document_client_capabilities_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_SYNCHRONIZATION)
    {
        json_t * p_synchronization_json = json_object();
        json_object_set_new(p_synchronization_json, "dynamicRegistration", encode_boolean(value.synchronization.dynamic_registration));
    json_object_set_new(p_synchronization_json, "willSave", encode_boolean(value.synchronization.will_save));
    json_object_set_new(p_synchronization_json, "willSaveWaitUntil", encode_boolean(value.synchronization.will_save_wait_until));
    json_object_set_new(p_synchronization_json, "didSave", encode_boolean(value.synchronization.did_save));
        json_object_set_new(p_json, "synchronization", p_synchronization_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_COMPLETION)
    {
        json_t * p_completion_json = json_object();
        json_object_set_new(p_completion_json, "snippetSupport", encode_boolean(value.completion.snippet_support));
    json_object_set_new(p_completion_json, "commitCharactersSupport", encode_boolean(value.completion.commit_characters_support));
        json_object_set_new(p_json, "completion", p_completion_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_HOVER)
    {
        json_t * p_hover_json = json_object();
        json_object_set_new(p_hover_json, "dynamicRegistration", encode_boolean(value.hover.dynamic_registration));
        json_object_set_new(p_json, "hover", p_hover_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_SIGNATURE_HELP)
    {
        json_t * p_signature_help_json = json_object();
        json_object_set_new(p_signature_help_json, "dynamicRegistration", encode_boolean(value.signature_help.dynamic_registration));
    json_t * p_signature_information_json = json_object();    json_t * p_documentation_format_json = json_array();    for (uint32_t i = 0; i < value.signature_help.signature_information.documentation_format_count; ++i)    {        json_array_append_new(p_documentation_format_json, encode_string(value.signature_help.signature_information.p_documentation_format[i]));    }    json_object_set_new(p_signature_information_json, "documentationFormat", p_documentation_format_json);    json_object_set_new(p_signature_help_json, "signatureInformation", p_signature_information_json);
        json_object_set_new(p_json, "signatureHelp", p_signature_help_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_REFERENCES)
    {
        json_t * p_references_json = json_object();
        json_object_set_new(p_references_json, "dynamicRegistration", encode_boolean(value.references.dynamic_registration));
        json_object_set_new(p_json, "references", p_references_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_HIGHLIGHT)
    {
        json_t * p_document_highlight_json = json_object();
        json_object_set_new(p_document_highlight_json, "dynamicRegistration", encode_boolean(value.document_highlight.dynamic_registration));
        json_object_set_new(p_json, "documentHighlight", p_document_highlight_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_SYMBOL)
    {
        json_t * p_document_symbol_json = json_object();
        json_object_set_new(p_document_symbol_json, "dynamicRegistration", encode_boolean(value.document_symbol.dynamic_registration));
        json_object_set_new(p_json, "documentSymbol", p_document_symbol_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_FORMATTING)
    {
        json_t * p_formatting_json = json_object();
        json_object_set_new(p_formatting_json, "dynamicRegistration", encode_boolean(value.formatting.dynamic_registration));
        json_object_set_new(p_json, "formatting", p_formatting_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_ON_TYPE_FORMATTING)
    {
        json_t * p_on_type_formatting_json = json_object();
        json_object_set_new(p_on_type_formatting_json, "dynamicRegistration", encode_boolean(value.on_type_formatting.dynamic_registration));
        json_object_set_new(p_json, "onTypeFormatting", p_on_type_formatting_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DEFINITION)
    {
        json_t * p_definition_json = json_object();
        json_object_set_new(p_definition_json, "dynamicRegistration", encode_boolean(value.definition.dynamic_registration));
        json_object_set_new(p_json, "definition", p_definition_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_CODE_ACTION)
    {
        json_t * p_code_action_json = json_object();
        json_object_set_new(p_code_action_json, "dynamicRegistration", encode_boolean(value.code_action.dynamic_registration));
        json_object_set_new(p_json, "codeAction", p_code_action_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_CODE_LENS)
    {
        json_t * p_code_lens_json = json_object();
        json_object_set_new(p_code_lens_json, "dynamicRegistration", encode_boolean(value.code_lens.dynamic_registration));
        json_object_set_new(p_json, "codeLens", p_code_lens_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_LINK)
    {
        json_t * p_document_link_json = json_object();
        json_object_set_new(p_document_link_json, "dynamicRegistration", encode_boolean(value.document_link.dynamic_registration));
        json_object_set_new(p_json, "documentLink", p_document_link_json);
    }

    if (value.valid_fields & TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_RENAME)
    {
        json_t * p_rename_json = json_object();
        json_object_set_new(p_rename_json, "dynamicRegistration", encode_boolean(value.rename.dynamic_registration));
        json_object_set_new(p_json, "rename", p_rename_json);
    }
    return p_json;
}

json_t * encode_client_capabilities(client_capabilities_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & CLIENT_CAPABILITIES_FIELD_WORKSPACE)
    {
        json_object_set_new(p_json, "workspace", encode_workspace_client_capabilities(value.workspace));
    }

    if (value.valid_fields & CLIENT_CAPABILITIES_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_client_capabilities(value.text_document));
    }

    if (value.valid_fields & CLIENT_CAPABILITIES_FIELD_EXPERIMENTAL)
    {
        json_object_set_new(p_json, "experimental", value.experimental);
    }
    return p_json;
}

json_t * encode_completion_options(completion_options_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & COMPLETION_OPTIONS_FIELD_RESOLVE_PROVIDER)
    {
        json_object_set_new(p_json, "resolveProvider", encode_boolean(value.resolve_provider));
    }

    if (value.valid_fields & COMPLETION_OPTIONS_FIELD_TRIGGER_CHARACTERS)
    {
        json_t * p_trigger_characters_json = json_array();
        for (uint32_t i = 0; i < value.trigger_characters_count; ++i)
        {
            json_array_append_new(p_trigger_characters_json, encode_string(value.p_trigger_characters[i]));
        }
        json_object_set_new(p_json, "triggerCharacters", p_trigger_characters_json);
    }
    return p_json;
}

json_t * encode_signature_help_options(signature_help_options_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SIGNATURE_HELP_OPTIONS_FIELD_TRIGGER_CHARACTERS)
    {
        json_t * p_trigger_characters_json = json_array();
        for (uint32_t i = 0; i < value.trigger_characters_count; ++i)
        {
            json_array_append_new(p_trigger_characters_json, encode_string(value.p_trigger_characters[i]));
        }
        json_object_set_new(p_json, "triggerCharacters", p_trigger_characters_json);
    }
    return p_json;
}

json_t * encode_code_lens_options(code_lens_options_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & CODE_LENS_OPTIONS_FIELD_RESOLVE_PROVIDER)
    {
        json_object_set_new(p_json, "resolveProvider", encode_boolean(value.resolve_provider));
    }
    return p_json;
}

json_t * encode_save_options(save_options_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SAVE_OPTIONS_FIELD_INCLUDE_TEXT)
    {
        json_object_set_new(p_json, "includeText", encode_boolean(value.include_text));
    }
    return p_json;
}

json_t * encode_text_document_sync_options(text_document_sync_options_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_OPEN_CLOSE)
    {
        json_object_set_new(p_json, "openClose", encode_boolean(value.open_close));
    }

    if (value.valid_fields & TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_CHANGE)
    {
        json_object_set_new(p_json, "change", encode_text_document_sync_kind(value.change));
    }

    if (value.valid_fields & TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_WILL_SAVE)
    {
        json_object_set_new(p_json, "willSave", encode_boolean(value.will_save));
    }

    if (value.valid_fields & TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_WILL_SAVE_WAIT_UNTIL)
    {
        json_object_set_new(p_json, "willSaveWaitUntil", encode_boolean(value.will_save_wait_until));
    }

    if (value.valid_fields & TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_SAVE)
    {
        json_object_set_new(p_json, "save", encode_save_options(value.save));
    }
    return p_json;
}

json_t * encode_server_capabilities(server_capabilities_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_TEXT_DOCUMENT_SYNC)
    {
        json_object_set_new(p_json, "textDocumentSync", encode_text_document_sync_options(value.text_document_sync));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_HOVER_PROVIDER)
    {
        json_object_set_new(p_json, "hoverProvider", encode_boolean(value.hover_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_COMPLETION_PROVIDER)
    {
        json_object_set_new(p_json, "completionProvider", encode_completion_options(value.completion_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_SIGNATURE_HELP_PROVIDER)
    {
        json_object_set_new(p_json, "signatureHelpProvider", encode_signature_help_options(value.signature_help_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_DEFINITION_PROVIDER)
    {
        json_object_set_new(p_json, "definitionProvider", encode_boolean(value.definition_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_REFERENCES_PROVIDER)
    {
        json_object_set_new(p_json, "referencesProvider", encode_boolean(value.references_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_DOCUMENT_HIGHLIGHT_PROVIDER)
    {
        json_object_set_new(p_json, "documentHighlightProvider", encode_boolean(value.document_highlight_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_DOCUMENT_SYMBOL_PROVIDER)
    {
        json_object_set_new(p_json, "documentSymbolProvider", encode_boolean(value.document_symbol_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_WORKSPACE_SYMBOL_PROVIDER)
    {
        json_object_set_new(p_json, "workspaceSymbolProvider", encode_boolean(value.workspace_symbol_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_CODE_ACTION_PROVIDER)
    {
        json_object_set_new(p_json, "codeActionProvider", encode_boolean(value.code_action_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_CODE_LENS_PROVIDER)
    {
        json_object_set_new(p_json, "codeLensProvider", encode_code_lens_options(value.code_lens_provider));
    }

    if (value.valid_fields & SERVER_CAPABILITIES_FIELD_DOCUMENT_FORMATTING_PROVIDER)
    {
        json_object_set_new(p_json, "documentFormattingProvider", encode_boolean(value.document_formatting_provider));
    }
    return p_json;
}

json_t * encode_markup_content(markup_content_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & MARKUP_CONTENT_FIELD_KIND)
    {
        json_object_set_new(p_json, "kind", encode_string(value.kind));
    }

    if (value.valid_fields & MARKUP_CONTENT_FIELD_VALUE)
    {
        json_object_set_new(p_json, "value", encode_string(value.value));
    }
    return p_json;
}

json_t * encode_marked_string(marked_string_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & MARKED_STRING_FIELD_LANGUAGE)
    {
        json_object_set_new(p_json, "language", encode_string(value.language));
    }

    if (value.valid_fields & MARKED_STRING_FIELD_VALUE)
    {
        json_object_set_new(p_json, "value", encode_string(value.value));
    }
    return p_json;
}

json_t * encode_message_action_item(message_action_item_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & MESSAGE_ACTION_ITEM_FIELD_TITLE)
    {
        json_object_set_new(p_json, "title", encode_string(value.title));
    }
    return p_json;
}

json_t * encode_position(position_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & POSITION_FIELD_LINE)
    {
        json_object_set_new(p_json, "line", encode_number(value.line));
    }

    if (value.valid_fields & POSITION_FIELD_CHARACTER)
    {
        json_object_set_new(p_json, "character", encode_number(value.character));
    }
    return p_json;
}

json_t * encode_range(range_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & RANGE_FIELD_START)
    {
        json_object_set_new(p_json, "start", encode_position(value.start));
    }

    if (value.valid_fields & RANGE_FIELD_END)
    {
        json_object_set_new(p_json, "end", encode_position(value.end));
    }
    return p_json;
}

json_t * encode_text_edit(text_edit_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_EDIT_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }

    if (value.valid_fields & TEXT_EDIT_FIELD_NEW_TEXT)
    {
        json_object_set_new(p_json, "newText", encode_string(value.new_text));
    }
    return p_json;
}

json_t * encode_diagnostic(diagnostic_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DIAGNOSTIC_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }

    if (value.valid_fields & DIAGNOSTIC_FIELD_SEVERITY)
    {
        json_object_set_new(p_json, "severity", encode_diagnostic_severity(value.severity));
    }

    if (value.valid_fields & DIAGNOSTIC_FIELD_CODE)
    {
        json_object_set_new(p_json, "code", encode_number(value.code));
    }

    if (value.valid_fields & DIAGNOSTIC_FIELD_SOURCE)
    {
        json_object_set_new(p_json, "source", encode_string(value.source));
    }

    if (value.valid_fields & DIAGNOSTIC_FIELD_MESSAGE)
    {
        json_object_set_new(p_json, "message", encode_string(value.message));
    }
    return p_json;
}

json_t * encode_location(location_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & LOCATION_FIELD_URI)
    {
        json_object_set_new(p_json, "uri", encode_uri(value.uri));
    }

    if (value.valid_fields & LOCATION_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }
    return p_json;
}

json_t * encode_text_document_item(text_document_item_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_DOCUMENT_ITEM_FIELD_URI)
    {
        json_object_set_new(p_json, "uri", encode_uri(value.uri));
    }

    if (value.valid_fields & TEXT_DOCUMENT_ITEM_FIELD_LANGUAGE_ID)
    {
        json_object_set_new(p_json, "languageId", encode_string(value.language_id));
    }

    if (value.valid_fields & TEXT_DOCUMENT_ITEM_FIELD_VERSION)
    {
        json_object_set_new(p_json, "version", encode_number(value.version));
    }

    if (value.valid_fields & TEXT_DOCUMENT_ITEM_FIELD_TEXT)
    {
        json_object_set_new(p_json, "text", encode_string(value.text));
    }
    return p_json;
}

json_t * encode_text_document_content_change_event(text_document_content_change_event_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }

    if (value.valid_fields & TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_RANGE_LENGTH)
    {
        json_object_set_new(p_json, "rangeLength", encode_number(value.range_length));
    }

    if (value.valid_fields & TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_TEXT)
    {
        json_object_set_new(p_json, "text", encode_string(value.text));
    }
    return p_json;
}

json_t * encode_text_document_identifier(text_document_identifier_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_DOCUMENT_IDENTIFIER_FIELD_URI)
    {
        json_object_set_new(p_json, "uri", encode_uri(value.uri));
    }
    return p_json;
}

json_t * encode_versioned_text_document_identifier(versioned_text_document_identifier_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_URI)
    {
        json_object_set_new(p_json, "uri", encode_uri(value.uri));
    }

    if (value.valid_fields & VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_VERSION)
    {
        json_object_set_new(p_json, "version", encode_number(value.version));
    }
    return p_json;
}

json_t * encode_file_event(file_event_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & FILE_EVENT_FIELD_URI)
    {
        json_object_set_new(p_json, "uri", encode_uri(value.uri));
    }

    if (value.valid_fields & FILE_EVENT_FIELD_TYPE)
    {
        json_object_set_new(p_json, "type", encode_file_change_type(value.type));
    }
    return p_json;
}

json_t * encode_text_document_position_params(text_document_position_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & TEXT_DOCUMENT_POSITION_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }

    if (value.valid_fields & TEXT_DOCUMENT_POSITION_PARAMS_FIELD_POSITION)
    {
        json_object_set_new(p_json, "position", encode_position(value.position));
    }
    return p_json;
}

json_t * encode_command(command_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & COMMAND_FIELD_TITLE)
    {
        json_object_set_new(p_json, "title", encode_string(value.title));
    }

    if (value.valid_fields & COMMAND_FIELD_COMMAND)
    {
        json_object_set_new(p_json, "command", encode_string(value.command));
    }

    if (value.valid_fields & COMMAND_FIELD_ARGUMENTS)
    {
        json_object_set_new(p_json, "arguments", value.arguments);
    }
    return p_json;
}

json_t * encode_completion_item(completion_item_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & COMPLETION_ITEM_FIELD_LABEL)
    {
        json_object_set_new(p_json, "label", encode_string(value.label));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_KIND)
    {
        json_object_set_new(p_json, "kind", encode_completion_item_kind(value.kind));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_DETAIL)
    {
        json_object_set_new(p_json, "detail", encode_string(value.detail));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_DOCUMENTATION)
    {
        json_object_set_new(p_json, "documentation", encode_markup_content(value.documentation));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_SORT_TEXT)
    {
        json_object_set_new(p_json, "sortText", encode_string(value.sort_text));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_FILTER_TEXT)
    {
        json_object_set_new(p_json, "filterText", encode_string(value.filter_text));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_INSERT_TEXT)
    {
        json_object_set_new(p_json, "insertText", encode_string(value.insert_text));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_INSERT_TEXT_FORMAT)
    {
        json_object_set_new(p_json, "insertTextFormat", encode_insert_text_format(value.insert_text_format));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_TEXT_EDIT)
    {
        json_object_set_new(p_json, "textEdit", encode_text_edit(value.text_edit));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_ADDITIONAL_TEXT_EDITS)
    {
        json_t * p_additional_text_edits_json = json_array();
        for (uint32_t i = 0; i < value.additional_text_edits_count; ++i)
        {
            json_array_append_new(p_additional_text_edits_json, encode_text_edit(value.p_additional_text_edits[i]));
        }
        json_object_set_new(p_json, "additionalTextEdits", p_additional_text_edits_json);
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_COMMIT_CHARACTERS)
    {
        json_t * p_commit_characters_json = json_array();
        for (uint32_t i = 0; i < value.commit_characters_count; ++i)
        {
            json_array_append_new(p_commit_characters_json, encode_string(value.p_commit_characters[i]));
        }
        json_object_set_new(p_json, "commitCharacters", p_commit_characters_json);
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_COMMAND)
    {
        json_object_set_new(p_json, "command", encode_command(value.command));
    }

    if (value.valid_fields & COMPLETION_ITEM_FIELD_DATA)
    {
        json_object_set_new(p_json, "data", value.data);
    }
    return p_json;
}

json_t * encode_parameter_information(parameter_information_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & PARAMETER_INFORMATION_FIELD_LABEL)
    {
        json_object_set_new(p_json, "label", encode_string(value.label));
    }

    if (value.valid_fields & PARAMETER_INFORMATION_FIELD_DOCUMENTATION)
    {
        json_object_set_new(p_json, "documentation", encode_markup_content(value.documentation));
    }
    return p_json;
}

json_t * encode_signature_information(signature_information_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SIGNATURE_INFORMATION_FIELD_LABEL)
    {
        json_object_set_new(p_json, "label", encode_string(value.label));
    }

    if (value.valid_fields & SIGNATURE_INFORMATION_FIELD_DOCUMENTATION)
    {
        json_object_set_new(p_json, "documentation", encode_markup_content(value.documentation));
    }

    if (value.valid_fields & SIGNATURE_INFORMATION_FIELD_PARAMETERS)
    {
        json_t * p_parameters_json = json_array();
        for (uint32_t i = 0; i < value.parameters_count; ++i)
        {
            json_array_append_new(p_parameters_json, encode_parameter_information(value.p_parameters[i]));
        }
        json_object_set_new(p_json, "parameters", p_parameters_json);
    }
    return p_json;
}

json_t * encode_reference_context(reference_context_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & REFERENCE_CONTEXT_FIELD_INCLUDE_DECLARATION)
    {
        json_object_set_new(p_json, "includeDeclaration", encode_boolean(value.include_declaration));
    }
    return p_json;
}

json_t * encode_completion_list(completion_list_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & COMPLETION_LIST_FIELD_IS_INCOMPLETE)
    {
        json_object_set_new(p_json, "isIncomplete", encode_boolean(value.is_incomplete));
    }

    if (value.valid_fields & COMPLETION_LIST_FIELD_ITEMS)
    {
        json_t * p_items_json = json_array();
        for (uint32_t i = 0; i < value.items_count; ++i)
        {
            json_array_append_new(p_items_json, encode_completion_item(value.p_items[i]));
        }
        json_object_set_new(p_json, "items", p_items_json);
    }
    return p_json;
}

json_t * encode_code_action_context(code_action_context_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & CODE_ACTION_CONTEXT_FIELD_DIAGNOSTICS)
    {
        json_t * p_diagnostics_json = json_array();
        for (uint32_t i = 0; i < value.diagnostics_count; ++i)
        {
            json_array_append_new(p_diagnostics_json, encode_diagnostic(value.p_diagnostics[i]));
        }
        json_object_set_new(p_json, "diagnostics", p_diagnostics_json);
    }
    return p_json;
}

/* Server command parameter JSON encoders */
json_t * encode_initialize_params(initialize_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & INITIALIZE_PARAMS_FIELD_PROCESS_ID)
    {
        json_object_set_new(p_json, "processId", encode_number(value.process_id));
    }

    if (value.valid_fields & INITIALIZE_PARAMS_FIELD_ROOT_PATH)
    {
        json_object_set_new(p_json, "rootPath", encode_string(value.root_path));
    }

    if (value.valid_fields & INITIALIZE_PARAMS_FIELD_ROOT_URI)
    {
        json_object_set_new(p_json, "rootUri", encode_uri(value.root_uri));
    }

    if (value.valid_fields & INITIALIZE_PARAMS_FIELD_CAPABILITIES)
    {
        json_object_set_new(p_json, "capabilities", encode_client_capabilities(value.capabilities));
    }

    if (value.valid_fields & INITIALIZE_PARAMS_FIELD_INITIALIZATION_OPTIONS)
    {
        json_object_set_new(p_json, "initializationOptions", encode_initialization_options(value.initialization_options));
    }

    if (value.valid_fields & INITIALIZE_PARAMS_FIELD_TRACE)
    {
        json_object_set_new(p_json, "trace", encode_string(value.trace));
    }
    return p_json;
}

json_t * encode_reference_params(reference_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & REFERENCE_PARAMS_FIELD_CONTEXT)
    {
        json_object_set_new(p_json, "context", encode_reference_context(value.context));
    }

    if (value.valid_fields & REFERENCE_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }

    if (value.valid_fields & REFERENCE_PARAMS_FIELD_POSITION)
    {
        json_object_set_new(p_json, "position", encode_position(value.position));
    }
    return p_json;
}

json_t * encode_document_symbol_params(document_symbol_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DOCUMENT_SYMBOL_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }
    return p_json;
}

json_t * encode_workspace_symbol_params(workspace_symbol_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & WORKSPACE_SYMBOL_PARAMS_FIELD_QUERY)
    {
        json_object_set_new(p_json, "query", encode_string(value.query));
    }
    return p_json;
}

json_t * encode_document_link_params(document_link_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DOCUMENT_LINK_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }
    return p_json;
}

json_t * encode_code_action_params(code_action_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & CODE_ACTION_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }

    if (value.valid_fields & CODE_ACTION_PARAMS_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }

    if (value.valid_fields & CODE_ACTION_PARAMS_FIELD_CONTEXT)
    {
        json_object_set_new(p_json, "context", encode_code_action_context(value.context));
    }
    return p_json;
}

/* Client command parameter JSON encoders */
json_t * encode_show_message_request_params(show_message_request_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SHOW_MESSAGE_REQUEST_PARAMS_FIELD_TYPE)
    {
        json_object_set_new(p_json, "type", encode_message_type(value.type));
    }

    if (value.valid_fields & SHOW_MESSAGE_REQUEST_PARAMS_FIELD_MESSAGE)
    {
        json_object_set_new(p_json, "message", encode_string(value.message));
    }

    if (value.valid_fields & SHOW_MESSAGE_REQUEST_PARAMS_FIELD_ACTIONS)
    {
        json_t * p_actions_json = json_array();
        for (uint32_t i = 0; i < value.actions_count; ++i)
        {
            json_array_append_new(p_actions_json, encode_message_action_item(value.p_actions[i]));
        }
        json_object_set_new(p_json, "actions", p_actions_json);
    }
    return p_json;
}

json_t * encode_signature_help(signature_help_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SIGNATURE_HELP_FIELD_SIGNATURES)
    {
        json_t * p_signatures_json = json_array();
        for (uint32_t i = 0; i < value.signatures_count; ++i)
        {
            json_array_append_new(p_signatures_json, encode_signature_information(value.p_signatures[i]));
        }
        json_object_set_new(p_json, "signatures", p_signatures_json);
    }

    if (value.valid_fields & SIGNATURE_HELP_FIELD_ACTIVE_SIGNATURE)
    {
        json_object_set_new(p_json, "activeSignature", encode_number(value.active_signature));
    }

    if (value.valid_fields & SIGNATURE_HELP_FIELD_ACTIVE_PARAMETER)
    {
        json_object_set_new(p_json, "activeParameter", encode_number(value.active_parameter));
    }
    return p_json;
}

/* Command response parameter JSON encoders */
json_t * encode_initialize_result(initialize_result_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & INITIALIZE_RESULT_FIELD_CAPABILITIES)
    {
        json_object_set_new(p_json, "capabilities", encode_server_capabilities(value.capabilities));
    }
    return p_json;
}

json_t * encode_symbol_information(symbol_information_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SYMBOL_INFORMATION_FIELD_NAME)
    {
        json_object_set_new(p_json, "name", encode_string(value.name));
    }

    if (value.valid_fields & SYMBOL_INFORMATION_FIELD_KIND)
    {
        json_object_set_new(p_json, "kind", encode_symbol_kind(value.kind));
    }

    if (value.valid_fields & SYMBOL_INFORMATION_FIELD_LOCATION)
    {
        json_object_set_new(p_json, "location", encode_location(value.location));
    }

    if (value.valid_fields & SYMBOL_INFORMATION_FIELD_CONTAINER_NAME)
    {
        json_object_set_new(p_json, "containerName", encode_string(value.container_name));
    }
    return p_json;
}

json_t * encode_document_link(document_link_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DOCUMENT_LINK_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }

    if (value.valid_fields & DOCUMENT_LINK_FIELD_TARGET)
    {
        json_object_set_new(p_json, "target", encode_uri(value.target));
    }
    return p_json;
}

json_t * encode_hover(hover_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & HOVER_FIELD_CONTENTS)
    {
        json_object_set_new(p_json, "contents", encode_markup_content(value.contents));
    }

    if (value.valid_fields & HOVER_FIELD_RANGE)
    {
        json_object_set_new(p_json, "range", encode_range(value.range));
    }
    return p_json;
}

/* Client notification parameter JSON encoders */
json_t * encode_show_message_params(show_message_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & SHOW_MESSAGE_PARAMS_FIELD_TYPE)
    {
        json_object_set_new(p_json, "type", encode_message_type(value.type));
    }

    if (value.valid_fields & SHOW_MESSAGE_PARAMS_FIELD_MESSAGE)
    {
        json_object_set_new(p_json, "message", encode_string(value.message));
    }
    return p_json;
}

json_t * encode_log_message_params(log_message_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & LOG_MESSAGE_PARAMS_FIELD_TYPE)
    {
        json_object_set_new(p_json, "type", encode_message_type(value.type));
    }

    if (value.valid_fields & LOG_MESSAGE_PARAMS_FIELD_MESSAGE)
    {
        json_object_set_new(p_json, "message", encode_string(value.message));
    }
    return p_json;
}

json_t * encode_publish_diagnostics_params(publish_diagnostics_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & PUBLISH_DIAGNOSTICS_PARAMS_FIELD_URI)
    {
        json_object_set_new(p_json, "uri", encode_uri(value.uri));
    }

    if (value.valid_fields & PUBLISH_DIAGNOSTICS_PARAMS_FIELD_DIAGNOSTICS)
    {
        json_t * p_diagnostics_json = json_array();
        for (uint32_t i = 0; i < value.diagnostics_count; ++i)
        {
            json_array_append_new(p_diagnostics_json, encode_diagnostic(value.p_diagnostics[i]));
        }
        json_object_set_new(p_json, "diagnostics", p_diagnostics_json);
    }
    return p_json;
}

/* Server notification parameter JSON encoders */
json_t * encode_did_open_text_document_params(did_open_text_document_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_item(value.text_document));
    }
    return p_json;
}

json_t * encode_did_change_text_document_params(did_change_text_document_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_versioned_text_document_identifier(value.text_document));
    }

    if (value.valid_fields & DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_CONTENT_CHANGES)
    {
        json_t * p_content_changes_json = json_array();
        for (uint32_t i = 0; i < value.content_changes_count; ++i)
        {
            json_array_append_new(p_content_changes_json, encode_text_document_content_change_event(value.p_content_changes[i]));
        }
        json_object_set_new(p_json, "contentChanges", p_content_changes_json);
    }
    return p_json;
}

json_t * encode_did_save_text_document_params(did_save_text_document_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }

    if (value.valid_fields & DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT)
    {
        json_object_set_new(p_json, "text", encode_string(value.text));
    }
    return p_json;
}

json_t * encode_did_close_text_document_params(did_close_text_document_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT)
    {
        json_object_set_new(p_json, "textDocument", encode_text_document_identifier(value.text_document));
    }
    return p_json;
}

json_t * encode_did_change_watched_files_params(did_change_watched_files_params_t value)
{
    json_t * p_json = json_object();

    if (value.valid_fields & DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_CHANGES)
    {
        json_t * p_changes_json = json_array();
        for (uint32_t i = 0; i < value.changes_count; ++i)
        {
            json_array_append_new(p_changes_json, encode_file_event(value.p_changes[i]));
        }
        json_object_set_new(p_json, "changes", p_changes_json);
    }
    return p_json;
}