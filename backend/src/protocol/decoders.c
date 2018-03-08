#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
#include "structures.h"

#include "decoders.h"
#include "log.h"

static decoder_error_t m_error;

/*******************************************************************************
 * Decoders
 ******************************************************************************/
/* Common parameter JSON decoders */
initialization_options_t decode_initialization_options(json_t * p_json)
{
    initialization_options_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_flags_json = json_object_get(p_json, "flags");
    if (json_is_array(p_flags_json))
    {
        retval.flags_count = json_array_size(p_flags_json);
        retval.p_flags = malloc(sizeof(char  *) * retval.flags_count);
        ASSERT(retval.p_flags);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_flags_json, index, p_it)
        {
            retval.p_flags[index] = decode_string(p_it);
        }
        retval.valid_fields |= INITIALIZATION_OPTIONS_FIELD_FLAGS;
    }
    json_decref(p_flags_json);


    json_t * p_compilation_database_json = json_object_get(p_json, "compilationDatabase");
    if (json_is_array(p_compilation_database_json))
    {
        retval.compilation_database_count = json_array_size(p_compilation_database_json);
        retval.p_compilation_database = malloc(sizeof(char  *) * retval.compilation_database_count);
        ASSERT(retval.p_compilation_database);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_compilation_database_json, index, p_it)
        {
            retval.p_compilation_database[index] = decode_string(p_it);
        }
        retval.valid_fields |= INITIALIZATION_OPTIONS_FIELD_COMPILATION_DATABASE;
    }
    json_decref(p_compilation_database_json);


    return retval;
}
workspace_client_capabilities_t decode_workspace_client_capabilities(json_t * p_json)
{
    workspace_client_capabilities_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_apply_edit_json = json_object_get(p_json, "applyEdit");
    if (json_is_boolean(p_apply_edit_json))
    {
        retval.apply_edit = decode_boolean(p_apply_edit_json);
        retval.valid_fields |= WORKSPACE_CLIENT_CAPABILITIES_FIELD_APPLY_EDIT;
    }
    json_decref(p_apply_edit_json);


    json_t * p_workspace_edit_json = json_object_get(p_json, "workspaceEdit");
    if (json_is_object(p_workspace_edit_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.workspace_edit.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= WORKSPACE_CLIENT_CAPABILITIES_FIELD_WORKSPACE_EDIT;
    }


    json_t * p_did_change_configuration_json = json_object_get(p_json, "didChangeConfiguration");
    if (json_is_object(p_did_change_configuration_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.did_change_configuration.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= WORKSPACE_CLIENT_CAPABILITIES_FIELD_DID_CHANGE_CONFIGURATION;
    }


    json_t * p_did_change_watched_files_json = json_object_get(p_json, "didChangeWatchedFiles");
    if (json_is_object(p_did_change_watched_files_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.did_change_watched_files.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= WORKSPACE_CLIENT_CAPABILITIES_FIELD_DID_CHANGE_WATCHED_FILES;
    }


    json_t * p_symbol_json = json_object_get(p_json, "symbol");
    if (json_is_object(p_symbol_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.symbol.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= WORKSPACE_CLIENT_CAPABILITIES_FIELD_SYMBOL;
    }


    json_t * p_execute_command_json = json_object_get(p_json, "executeCommand");
    if (json_is_object(p_execute_command_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.execute_command.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= WORKSPACE_CLIENT_CAPABILITIES_FIELD_EXECUTE_COMMAND;
    }


    return retval;
}
text_document_client_capabilities_t decode_text_document_client_capabilities(json_t * p_json)
{
    text_document_client_capabilities_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_synchronization_json = json_object_get(p_json, "synchronization");
    if (json_is_object(p_synchronization_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.synchronization.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);


        json_t * p_will_save_json = json_object_get(p_json, "willSave");
        if (json_is_boolean(p_will_save_json))
        {
            retval.synchronization.will_save = decode_boolean(p_will_save_json);
        }
        json_decref(p_will_save_json);


        json_t * p_will_save_wait_until_json = json_object_get(p_json, "willSaveWaitUntil");
        if (json_is_boolean(p_will_save_wait_until_json))
        {
            retval.synchronization.will_save_wait_until = decode_boolean(p_will_save_wait_until_json);
        }
        json_decref(p_will_save_wait_until_json);


        json_t * p_did_save_json = json_object_get(p_json, "didSave");
        if (json_is_boolean(p_did_save_json))
        {
            retval.synchronization.did_save = decode_boolean(p_did_save_json);
        }
        json_decref(p_did_save_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_SYNCHRONIZATION;
    }


    json_t * p_completion_json = json_object_get(p_json, "completion");
    if (json_is_object(p_completion_json))
    {
        json_t * p_snippet_support_json = json_object_get(p_json, "snippetSupport");
        if (json_is_boolean(p_snippet_support_json))
        {
            retval.completion.snippet_support = decode_boolean(p_snippet_support_json);
        }
        json_decref(p_snippet_support_json);


        json_t * p_commit_characters_support_json = json_object_get(p_json, "commitCharactersSupport");
        if (json_is_boolean(p_commit_characters_support_json))
        {
            retval.completion.commit_characters_support = decode_boolean(p_commit_characters_support_json);
        }
        json_decref(p_commit_characters_support_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_COMPLETION;
    }


    json_t * p_hover_json = json_object_get(p_json, "hover");
    if (json_is_object(p_hover_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.hover.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_HOVER;
    }


    json_t * p_signature_help_json = json_object_get(p_json, "signatureHelp");
    if (json_is_object(p_signature_help_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.signature_help.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);


        json_t * p_signature_information_json = json_object_get(p_json, "signatureInformation");
        if (json_is_object(p_signature_information_json))
        {
            json_t * p_documentation_format_json = json_object_get(p_json, "documentationFormat");
            if (json_is_array(p_documentation_format_json))
            {
                retval.signature_help.signature_information.documentation_format_count = json_array_size(p_documentation_format_json);
                retval.signature_help.signature_information.p_documentation_format = malloc(sizeof(char  *) * retval.signature_help.signature_information.documentation_format_count);
                ASSERT(retval.signature_help.signature_information.p_documentation_format);
                json_t * p_it;
                uint32_t index;
                json_array_foreach(p_documentation_format_json, index, p_it)
                {
                    retval.signature_help.signature_information.p_documentation_format[index] = decode_string(p_it);
                }
            }
            json_decref(p_documentation_format_json);

        }

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_SIGNATURE_HELP;
    }


    json_t * p_references_json = json_object_get(p_json, "references");
    if (json_is_object(p_references_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.references.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_REFERENCES;
    }


    json_t * p_document_highlight_json = json_object_get(p_json, "documentHighlight");
    if (json_is_object(p_document_highlight_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.document_highlight.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_HIGHLIGHT;
    }


    json_t * p_document_symbol_json = json_object_get(p_json, "documentSymbol");
    if (json_is_object(p_document_symbol_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.document_symbol.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_SYMBOL;
    }


    json_t * p_formatting_json = json_object_get(p_json, "formatting");
    if (json_is_object(p_formatting_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.formatting.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_FORMATTING;
    }


    json_t * p_on_type_formatting_json = json_object_get(p_json, "onTypeFormatting");
    if (json_is_object(p_on_type_formatting_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.on_type_formatting.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_ON_TYPE_FORMATTING;
    }


    json_t * p_definition_json = json_object_get(p_json, "definition");
    if (json_is_object(p_definition_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.definition.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DEFINITION;
    }


    json_t * p_code_action_json = json_object_get(p_json, "codeAction");
    if (json_is_object(p_code_action_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.code_action.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_CODE_ACTION;
    }


    json_t * p_code_lens_json = json_object_get(p_json, "codeLens");
    if (json_is_object(p_code_lens_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.code_lens.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_CODE_LENS;
    }


    json_t * p_document_link_json = json_object_get(p_json, "documentLink");
    if (json_is_object(p_document_link_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.document_link.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_LINK;
    }


    json_t * p_rename_json = json_object_get(p_json, "rename");
    if (json_is_object(p_rename_json))
    {
        json_t * p_dynamic_registration_json = json_object_get(p_json, "dynamicRegistration");
        if (json_is_boolean(p_dynamic_registration_json))
        {
            retval.rename.dynamic_registration = decode_boolean(p_dynamic_registration_json);
        }
        json_decref(p_dynamic_registration_json);

        retval.valid_fields |= TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_RENAME;
    }


    return retval;
}
client_capabilities_t decode_client_capabilities(json_t * p_json)
{
    client_capabilities_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_workspace_json = json_object_get(p_json, "workspace");
    if (json_is_object(p_workspace_json))
    {
        retval.workspace = decode_workspace_client_capabilities(p_workspace_json);
        retval.valid_fields |= CLIENT_CAPABILITIES_FIELD_WORKSPACE;
    }
    json_decref(p_workspace_json);


    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_client_capabilities(p_text_document_json);
        retval.valid_fields |= CLIENT_CAPABILITIES_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    json_t * p_experimenal_json = json_object_get(p_json, "experimenal");
    json_incref(p_experimenal_json);
    retval.experimenal = p_experimenal_json;
    if (p_experimenal_json != NULL)
    {
        retval.valid_fields |= CLIENT_CAPABILITIES_FIELD_EXPERIMENAL;
    }


    return retval;
}
completion_options_t decode_completion_options(json_t * p_json)
{
    completion_options_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_resolve_provider_json = json_object_get(p_json, "resolveProvider");
    if (json_is_boolean(p_resolve_provider_json))
    {
        retval.resolve_provider = decode_boolean(p_resolve_provider_json);
        retval.valid_fields |= COMPLETION_OPTIONS_FIELD_RESOLVE_PROVIDER;
    }
    json_decref(p_resolve_provider_json);


    json_t * p_trigger_characters_json = json_object_get(p_json, "triggerCharacters");
    if (json_is_array(p_trigger_characters_json))
    {
        retval.trigger_characters_count = json_array_size(p_trigger_characters_json);
        retval.p_trigger_characters = malloc(sizeof(char  *) * retval.trigger_characters_count);
        ASSERT(retval.p_trigger_characters);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_trigger_characters_json, index, p_it)
        {
            retval.p_trigger_characters[index] = decode_string(p_it);
        }
        retval.valid_fields |= COMPLETION_OPTIONS_FIELD_TRIGGER_CHARACTERS;
    }
    json_decref(p_trigger_characters_json);


    return retval;
}
signature_help_options_t decode_signature_help_options(json_t * p_json)
{
    signature_help_options_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_trigger_characters_json = json_object_get(p_json, "triggerCharacters");
    if (json_is_array(p_trigger_characters_json))
    {
        retval.trigger_characters_count = json_array_size(p_trigger_characters_json);
        retval.p_trigger_characters = malloc(sizeof(char  *) * retval.trigger_characters_count);
        ASSERT(retval.p_trigger_characters);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_trigger_characters_json, index, p_it)
        {
            retval.p_trigger_characters[index] = decode_string(p_it);
        }
        retval.valid_fields |= SIGNATURE_HELP_OPTIONS_FIELD_TRIGGER_CHARACTERS;
    }
    json_decref(p_trigger_characters_json);


    return retval;
}
code_lens_options_t decode_code_lens_options(json_t * p_json)
{
    code_lens_options_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_resolve_provider_json = json_object_get(p_json, "resolveProvider");
    if (json_is_boolean(p_resolve_provider_json))
    {
        retval.resolve_provider = decode_boolean(p_resolve_provider_json);
        retval.valid_fields |= CODE_LENS_OPTIONS_FIELD_RESOLVE_PROVIDER;
    }
    json_decref(p_resolve_provider_json);


    return retval;
}
save_options_t decode_save_options(json_t * p_json)
{
    save_options_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_include_text_json = json_object_get(p_json, "includeText");
    if (json_is_boolean(p_include_text_json))
    {
        retval.include_text = decode_boolean(p_include_text_json);
        retval.valid_fields |= SAVE_OPTIONS_FIELD_INCLUDE_TEXT;
    }
    json_decref(p_include_text_json);


    return retval;
}
text_document_sync_options_t decode_text_document_sync_options(json_t * p_json)
{
    text_document_sync_options_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_open_close_json = json_object_get(p_json, "openClose");
    if (json_is_boolean(p_open_close_json))
    {
        retval.open_close = decode_boolean(p_open_close_json);
        retval.valid_fields |= TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_OPEN_CLOSE;
    }
    json_decref(p_open_close_json);


    json_t * p_change_json = json_object_get(p_json, "change");
    if (json_is_object(p_change_json))
    {
        retval.change = decode_text_document_sync_kind(p_change_json);
        retval.valid_fields |= TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_CHANGE;
    }
    json_decref(p_change_json);


    json_t * p_will_save_json = json_object_get(p_json, "willSave");
    if (json_is_boolean(p_will_save_json))
    {
        retval.will_save = decode_boolean(p_will_save_json);
        retval.valid_fields |= TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_WILL_SAVE;
    }
    json_decref(p_will_save_json);


    json_t * p_will_save_wait_until_json = json_object_get(p_json, "willSaveWaitUntil");
    if (json_is_boolean(p_will_save_wait_until_json))
    {
        retval.will_save_wait_until = decode_boolean(p_will_save_wait_until_json);
        retval.valid_fields |= TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_WILL_SAVE_WAIT_UNTIL;
    }
    json_decref(p_will_save_wait_until_json);


    json_t * p_save_json = json_object_get(p_json, "save");
    if (json_is_object(p_save_json))
    {
        retval.save = decode_save_options(p_save_json);
        retval.valid_fields |= TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_SAVE;
    }
    json_decref(p_save_json);


    return retval;
}
server_capabilities_t decode_server_capabilities(json_t * p_json)
{
    server_capabilities_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_sync_json = json_object_get(p_json, "textDocumentSync");
    if (json_is_object(p_text_document_sync_json))
    {
        retval.text_document_sync = decode_text_document_sync_options(p_text_document_sync_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_TEXT_DOCUMENT_SYNC;
    }
    json_decref(p_text_document_sync_json);


    json_t * p_hover_provider_json = json_object_get(p_json, "hoverProvider");
    if (json_is_boolean(p_hover_provider_json))
    {
        retval.hover_provider = decode_boolean(p_hover_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_HOVER_PROVIDER;
    }
    json_decref(p_hover_provider_json);


    json_t * p_completion_provider_json = json_object_get(p_json, "completionProvider");
    if (json_is_object(p_completion_provider_json))
    {
        retval.completion_provider = decode_completion_options(p_completion_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_COMPLETION_PROVIDER;
    }
    json_decref(p_completion_provider_json);


    json_t * p_signature_help_provider_json = json_object_get(p_json, "signatureHelpProvider");
    if (json_is_object(p_signature_help_provider_json))
    {
        retval.signature_help_provider = decode_signature_help_options(p_signature_help_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_SIGNATURE_HELP_PROVIDER;
    }
    json_decref(p_signature_help_provider_json);


    json_t * p_definition_provider_json = json_object_get(p_json, "definitionProvider");
    if (json_is_boolean(p_definition_provider_json))
    {
        retval.definition_provider = decode_boolean(p_definition_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_DEFINITION_PROVIDER;
    }
    json_decref(p_definition_provider_json);


    json_t * p_references_provider_json = json_object_get(p_json, "referencesProvider");
    if (json_is_boolean(p_references_provider_json))
    {
        retval.references_provider = decode_boolean(p_references_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_REFERENCES_PROVIDER;
    }
    json_decref(p_references_provider_json);


    json_t * p_document_highlight_provider_json = json_object_get(p_json, "documentHighlightProvider");
    if (json_is_boolean(p_document_highlight_provider_json))
    {
        retval.document_highlight_provider = decode_boolean(p_document_highlight_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_DOCUMENT_HIGHLIGHT_PROVIDER;
    }
    json_decref(p_document_highlight_provider_json);


    json_t * p_document_symbol_provider_json = json_object_get(p_json, "documentSymbolProvider");
    if (json_is_boolean(p_document_symbol_provider_json))
    {
        retval.document_symbol_provider = decode_boolean(p_document_symbol_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_DOCUMENT_SYMBOL_PROVIDER;
    }
    json_decref(p_document_symbol_provider_json);


    json_t * p_workspace_symbol_provider_json = json_object_get(p_json, "workspaceSymbolProvider");
    if (json_is_boolean(p_workspace_symbol_provider_json))
    {
        retval.workspace_symbol_provider = decode_boolean(p_workspace_symbol_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_WORKSPACE_SYMBOL_PROVIDER;
    }
    json_decref(p_workspace_symbol_provider_json);


    json_t * p_code_action_provider_json = json_object_get(p_json, "codeActionProvider");
    if (json_is_boolean(p_code_action_provider_json))
    {
        retval.code_action_provider = decode_boolean(p_code_action_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_CODE_ACTION_PROVIDER;
    }
    json_decref(p_code_action_provider_json);


    json_t * p_code_lens_provider_json = json_object_get(p_json, "codeLensProvider");
    if (json_is_object(p_code_lens_provider_json))
    {
        retval.code_lens_provider = decode_code_lens_options(p_code_lens_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_CODE_LENS_PROVIDER;
    }
    json_decref(p_code_lens_provider_json);


    json_t * p_document_formatting_provider_json = json_object_get(p_json, "documentFormattingProvider");
    if (json_is_boolean(p_document_formatting_provider_json))
    {
        retval.document_formatting_provider = decode_boolean(p_document_formatting_provider_json);
        retval.valid_fields |= SERVER_CAPABILITIES_FIELD_DOCUMENT_FORMATTING_PROVIDER;
    }
    json_decref(p_document_formatting_provider_json);


    return retval;
}
markup_content_t decode_markup_content(json_t * p_json)
{
    markup_content_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_kind_json = json_object_get(p_json, "kind");
    if (json_is_string(p_kind_json))
    {
        retval.kind = decode_string(p_kind_json);
        retval.valid_fields |= MARKUP_CONTENT_FIELD_KIND;
    }
    json_decref(p_kind_json);


    json_t * p_value_json = json_object_get(p_json, "value");
    if (json_is_string(p_value_json))
    {
        retval.value = decode_string(p_value_json);
        retval.valid_fields |= MARKUP_CONTENT_FIELD_VALUE;
    }
    json_decref(p_value_json);


    if ((retval.valid_fields & MARKUP_CONTENT_FIELD_REQUIRED) != MARKUP_CONTENT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for markup_content: Got 0x%x, expected 0x%x\n", retval.valid_fields, MARKUP_CONTENT_FIELD_REQUIRED);
    }
    return retval;
}
marked_string_t decode_marked_string(json_t * p_json)
{
    marked_string_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_language_json = json_object_get(p_json, "language");
    if (json_is_string(p_language_json))
    {
        retval.language = decode_string(p_language_json);
        retval.valid_fields |= MARKED_STRING_FIELD_LANGUAGE;
    }
    json_decref(p_language_json);


    json_t * p_value_json = json_object_get(p_json, "value");
    if (json_is_string(p_value_json))
    {
        retval.value = decode_string(p_value_json);
        retval.valid_fields |= MARKED_STRING_FIELD_VALUE;
    }
    json_decref(p_value_json);


    if ((retval.valid_fields & MARKED_STRING_FIELD_REQUIRED) != MARKED_STRING_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for marked_string: Got 0x%x, expected 0x%x\n", retval.valid_fields, MARKED_STRING_FIELD_REQUIRED);
    }
    return retval;
}
message_action_item_t decode_message_action_item(json_t * p_json)
{
    message_action_item_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_title_json = json_object_get(p_json, "title");
    if (json_is_string(p_title_json))
    {
        retval.title = decode_string(p_title_json);
        retval.valid_fields |= MESSAGE_ACTION_ITEM_FIELD_TITLE;
    }
    json_decref(p_title_json);


    if ((retval.valid_fields & MESSAGE_ACTION_ITEM_FIELD_REQUIRED) != MESSAGE_ACTION_ITEM_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for message_action_item: Got 0x%x, expected 0x%x\n", retval.valid_fields, MESSAGE_ACTION_ITEM_FIELD_REQUIRED);
    }
    return retval;
}
position_t decode_position(json_t * p_json)
{
    position_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_line_json = json_object_get(p_json, "line");
    if (json_is_integer(p_line_json))
    {
        retval.line = decode_number(p_line_json);
        retval.valid_fields |= POSITION_FIELD_LINE;
    }
    json_decref(p_line_json);


    json_t * p_character_json = json_object_get(p_json, "character");
    if (json_is_integer(p_character_json))
    {
        retval.character = decode_number(p_character_json);
        retval.valid_fields |= POSITION_FIELD_CHARACTER;
    }
    json_decref(p_character_json);


    if ((retval.valid_fields & POSITION_FIELD_REQUIRED) != POSITION_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for position: Got 0x%x, expected 0x%x\n", retval.valid_fields, POSITION_FIELD_REQUIRED);
    }
    return retval;
}
range_t decode_range(json_t * p_json)
{
    range_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_start_json = json_object_get(p_json, "start");
    if (json_is_object(p_start_json))
    {
        retval.start = decode_position(p_start_json);
        retval.valid_fields |= RANGE_FIELD_START;
    }
    json_decref(p_start_json);


    json_t * p_end_json = json_object_get(p_json, "end");
    if (json_is_object(p_end_json))
    {
        retval.end = decode_position(p_end_json);
        retval.valid_fields |= RANGE_FIELD_END;
    }
    json_decref(p_end_json);


    if ((retval.valid_fields & RANGE_FIELD_REQUIRED) != RANGE_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for range: Got 0x%x, expected 0x%x\n", retval.valid_fields, RANGE_FIELD_REQUIRED);
    }
    return retval;
}
text_edit_t decode_text_edit(json_t * p_json)
{
    text_edit_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= TEXT_EDIT_FIELD_RANGE;
    }
    json_decref(p_range_json);


    json_t * p_new_text_json = json_object_get(p_json, "newText");
    if (json_is_string(p_new_text_json))
    {
        retval.new_text = decode_string(p_new_text_json);
        retval.valid_fields |= TEXT_EDIT_FIELD_NEW_TEXT;
    }
    json_decref(p_new_text_json);


    if ((retval.valid_fields & TEXT_EDIT_FIELD_REQUIRED) != TEXT_EDIT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for text_edit: Got 0x%x, expected 0x%x\n", retval.valid_fields, TEXT_EDIT_FIELD_REQUIRED);
    }
    return retval;
}
diagnostic_t decode_diagnostic(json_t * p_json)
{
    diagnostic_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= DIAGNOSTIC_FIELD_RANGE;
    }
    json_decref(p_range_json);


    json_t * p_severity_json = json_object_get(p_json, "severity");
    if (json_is_object(p_severity_json))
    {
        retval.severity = decode_diagnostic_severity(p_severity_json);
        retval.valid_fields |= DIAGNOSTIC_FIELD_SEVERITY;
    }
    json_decref(p_severity_json);


    json_t * p_code_json = json_object_get(p_json, "code");
    if (json_is_integer(p_code_json))
    {
        retval.code = decode_number(p_code_json);
        retval.valid_fields |= DIAGNOSTIC_FIELD_CODE;
    }
    json_decref(p_code_json);


    json_t * p_source_json = json_object_get(p_json, "source");
    if (json_is_string(p_source_json))
    {
        retval.source = decode_string(p_source_json);
        retval.valid_fields |= DIAGNOSTIC_FIELD_SOURCE;
    }
    json_decref(p_source_json);


    json_t * p_message_json = json_object_get(p_json, "message");
    if (json_is_string(p_message_json))
    {
        retval.message = decode_string(p_message_json);
        retval.valid_fields |= DIAGNOSTIC_FIELD_MESSAGE;
    }
    json_decref(p_message_json);


    if ((retval.valid_fields & DIAGNOSTIC_FIELD_REQUIRED) != DIAGNOSTIC_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for diagnostic: Got 0x%x, expected 0x%x\n", retval.valid_fields, DIAGNOSTIC_FIELD_REQUIRED);
    }
    return retval;
}
location_t decode_location(json_t * p_json)
{
    location_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_uri_json = json_object_get(p_json, "uri");
    if (json_is_string(p_uri_json))
    {
        retval.uri = decode_uri(p_uri_json);
        retval.valid_fields |= LOCATION_FIELD_URI;
    }
    json_decref(p_uri_json);


    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= LOCATION_FIELD_RANGE;
    }
    json_decref(p_range_json);


    if ((retval.valid_fields & LOCATION_FIELD_REQUIRED) != LOCATION_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for location: Got 0x%x, expected 0x%x\n", retval.valid_fields, LOCATION_FIELD_REQUIRED);
    }
    return retval;
}
text_document_item_t decode_text_document_item(json_t * p_json)
{
    text_document_item_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_uri_json = json_object_get(p_json, "uri");
    if (json_is_string(p_uri_json))
    {
        retval.uri = decode_uri(p_uri_json);
        retval.valid_fields |= TEXT_DOCUMENT_ITEM_FIELD_URI;
    }
    json_decref(p_uri_json);


    json_t * p_language_id_json = json_object_get(p_json, "languageId");
    if (json_is_string(p_language_id_json))
    {
        retval.language_id = decode_string(p_language_id_json);
        retval.valid_fields |= TEXT_DOCUMENT_ITEM_FIELD_LANGUAGE_ID;
    }
    json_decref(p_language_id_json);


    json_t * p_version_json = json_object_get(p_json, "version");
    if (json_is_integer(p_version_json))
    {
        retval.version = decode_number(p_version_json);
        retval.valid_fields |= TEXT_DOCUMENT_ITEM_FIELD_VERSION;
    }
    json_decref(p_version_json);


    json_t * p_text_json = json_object_get(p_json, "text");
    if (json_is_string(p_text_json))
    {
        retval.text = decode_string(p_text_json);
        retval.valid_fields |= TEXT_DOCUMENT_ITEM_FIELD_TEXT;
    }
    json_decref(p_text_json);


    if ((retval.valid_fields & TEXT_DOCUMENT_ITEM_FIELD_REQUIRED) != TEXT_DOCUMENT_ITEM_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for text_document_item: Got 0x%x, expected 0x%x\n", retval.valid_fields, TEXT_DOCUMENT_ITEM_FIELD_REQUIRED);
    }
    return retval;
}
text_document_content_change_event_t decode_text_document_content_change_event(json_t * p_json)
{
    text_document_content_change_event_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_RANGE;
    }
    json_decref(p_range_json);


    json_t * p_range_length_json = json_object_get(p_json, "rangeLength");
    if (json_is_integer(p_range_length_json))
    {
        retval.range_length = decode_number(p_range_length_json);
        retval.valid_fields |= TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_RANGE_LENGTH;
    }
    json_decref(p_range_length_json);


    json_t * p_text_json = json_object_get(p_json, "text");
    if (json_is_string(p_text_json))
    {
        retval.text = decode_string(p_text_json);
        retval.valid_fields |= TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_TEXT;
    }
    json_decref(p_text_json);


    if ((retval.valid_fields & TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_REQUIRED) != TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for text_document_content_change_event: Got 0x%x, expected 0x%x\n", retval.valid_fields, TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_REQUIRED);
    }
    return retval;
}
text_document_identifier_t decode_text_document_identifier(json_t * p_json)
{
    text_document_identifier_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_uri_json = json_object_get(p_json, "uri");
    if (json_is_string(p_uri_json))
    {
        retval.uri = decode_uri(p_uri_json);
        retval.valid_fields |= TEXT_DOCUMENT_IDENTIFIER_FIELD_URI;
    }
    json_decref(p_uri_json);


    if ((retval.valid_fields & TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED) != TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for text_document_identifier: Got 0x%x, expected 0x%x\n", retval.valid_fields, TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED);
    }
    return retval;
}
versioned_text_document_identifier_t decode_versioned_text_document_identifier(json_t * p_json)
{
    versioned_text_document_identifier_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_uri_json = json_object_get(p_json, "uri");
    if (json_is_string(p_uri_json))
    {
        retval.uri = decode_uri(p_uri_json);
        retval.valid_fields |= VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_URI;
    }
    json_decref(p_uri_json);


    json_t * p_version_json = json_object_get(p_json, "version");
    if (json_is_integer(p_version_json))
    {
        retval.version = decode_number(p_version_json);
        retval.valid_fields |= VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_VERSION;
    }
    json_decref(p_version_json);


    if ((retval.valid_fields & VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED) != VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for versioned_text_document_identifier: Got 0x%x, expected 0x%x\n", retval.valid_fields, VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED);
    }
    return retval;
}
file_event_t decode_file_event(json_t * p_json)
{
    file_event_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_uri_json = json_object_get(p_json, "uri");
    if (json_is_string(p_uri_json))
    {
        retval.uri = decode_uri(p_uri_json);
        retval.valid_fields |= FILE_EVENT_FIELD_URI;
    }
    json_decref(p_uri_json);


    json_t * p_type_json = json_object_get(p_json, "type");
    if (json_is_object(p_type_json))
    {
        retval.type = decode_file_change_type(p_type_json);
        retval.valid_fields |= FILE_EVENT_FIELD_TYPE;
    }
    json_decref(p_type_json);


    if ((retval.valid_fields & FILE_EVENT_FIELD_REQUIRED) != FILE_EVENT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for file_event: Got 0x%x, expected 0x%x\n", retval.valid_fields, FILE_EVENT_FIELD_REQUIRED);
    }
    return retval;
}
text_document_position_params_t decode_text_document_position_params(json_t * p_json)
{
    text_document_position_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= TEXT_DOCUMENT_POSITION_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    json_t * p_position_json = json_object_get(p_json, "position");
    if (json_is_object(p_position_json))
    {
        retval.position = decode_position(p_position_json);
        retval.valid_fields |= TEXT_DOCUMENT_POSITION_PARAMS_FIELD_POSITION;
    }
    json_decref(p_position_json);


    if ((retval.valid_fields & TEXT_DOCUMENT_POSITION_PARAMS_FIELD_REQUIRED) != TEXT_DOCUMENT_POSITION_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for text_document_position_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, TEXT_DOCUMENT_POSITION_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
command_t decode_command(json_t * p_json)
{
    command_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_title_json = json_object_get(p_json, "title");
    if (json_is_string(p_title_json))
    {
        retval.title = decode_string(p_title_json);
        retval.valid_fields |= COMMAND_FIELD_TITLE;
    }
    json_decref(p_title_json);


    json_t * p_command_json = json_object_get(p_json, "command");
    if (json_is_string(p_command_json))
    {
        retval.command = decode_string(p_command_json);
        retval.valid_fields |= COMMAND_FIELD_COMMAND;
    }
    json_decref(p_command_json);


    json_t * p_arguments_json = json_object_get(p_json, "arguments");
    json_incref(p_arguments_json);
    retval.arguments = p_arguments_json;
    if (p_arguments_json != NULL)
    {
        retval.valid_fields |= COMMAND_FIELD_ARGUMENTS;
    }


    if ((retval.valid_fields & COMMAND_FIELD_REQUIRED) != COMMAND_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for command: Got 0x%x, expected 0x%x\n", retval.valid_fields, COMMAND_FIELD_REQUIRED);
    }
    return retval;
}
completion_item_t decode_completion_item(json_t * p_json)
{
    completion_item_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_label_json = json_object_get(p_json, "label");
    if (json_is_string(p_label_json))
    {
        retval.label = decode_string(p_label_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_LABEL;
    }
    json_decref(p_label_json);


    json_t * p_kind_json = json_object_get(p_json, "kind");
    if (json_is_object(p_kind_json))
    {
        retval.kind = decode_completion_item_kind(p_kind_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_KIND;
    }
    json_decref(p_kind_json);


    json_t * p_detail_json = json_object_get(p_json, "detail");
    if (json_is_string(p_detail_json))
    {
        retval.detail = decode_string(p_detail_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_DETAIL;
    }
    json_decref(p_detail_json);


    json_t * p_documentation_json = json_object_get(p_json, "documentation");
    if (json_is_object(p_documentation_json))
    {
        retval.documentation = decode_markup_content(p_documentation_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_DOCUMENTATION;
    }
    json_decref(p_documentation_json);


    json_t * p_sort_text_json = json_object_get(p_json, "sortText");
    if (json_is_string(p_sort_text_json))
    {
        retval.sort_text = decode_string(p_sort_text_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_SORT_TEXT;
    }
    json_decref(p_sort_text_json);


    json_t * p_filter_text_json = json_object_get(p_json, "filterText");
    if (json_is_string(p_filter_text_json))
    {
        retval.filter_text = decode_string(p_filter_text_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_FILTER_TEXT;
    }
    json_decref(p_filter_text_json);


    json_t * p_insert_text_json = json_object_get(p_json, "insertText");
    if (json_is_string(p_insert_text_json))
    {
        retval.insert_text = decode_string(p_insert_text_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_INSERT_TEXT;
    }
    json_decref(p_insert_text_json);


    json_t * p_insert_text_format_json = json_object_get(p_json, "insertTextFormat");
    if (json_is_object(p_insert_text_format_json))
    {
        retval.insert_text_format = decode_insert_text_format(p_insert_text_format_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_INSERT_TEXT_FORMAT;
    }
    json_decref(p_insert_text_format_json);


    json_t * p_text_edit_json = json_object_get(p_json, "textEdit");
    if (json_is_object(p_text_edit_json))
    {
        retval.text_edit = decode_text_edit(p_text_edit_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_TEXT_EDIT;
    }
    json_decref(p_text_edit_json);


    json_t * p_additional_text_edits_json = json_object_get(p_json, "additionalTextEdits");
    if (json_is_array(p_additional_text_edits_json))
    {
        retval.additional_text_edits_count = json_array_size(p_additional_text_edits_json);
        retval.p_additional_text_edits = malloc(sizeof(text_edit_t) * retval.additional_text_edits_count);
        ASSERT(retval.p_additional_text_edits);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_additional_text_edits_json, index, p_it)
        {
            retval.p_additional_text_edits[index] = decode_text_edit(p_it);
        }
        retval.valid_fields |= COMPLETION_ITEM_FIELD_ADDITIONAL_TEXT_EDITS;
    }
    json_decref(p_additional_text_edits_json);


    json_t * p_commit_characters_json = json_object_get(p_json, "commitCharacters");
    if (json_is_array(p_commit_characters_json))
    {
        retval.commit_characters_count = json_array_size(p_commit_characters_json);
        retval.p_commit_characters = malloc(sizeof(char  *) * retval.commit_characters_count);
        ASSERT(retval.p_commit_characters);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_commit_characters_json, index, p_it)
        {
            retval.p_commit_characters[index] = decode_string(p_it);
        }
        retval.valid_fields |= COMPLETION_ITEM_FIELD_COMMIT_CHARACTERS;
    }
    json_decref(p_commit_characters_json);


    json_t * p_command_json = json_object_get(p_json, "command");
    if (json_is_object(p_command_json))
    {
        retval.command = decode_command(p_command_json);
        retval.valid_fields |= COMPLETION_ITEM_FIELD_COMMAND;
    }
    json_decref(p_command_json);


    json_t * p_data_json = json_object_get(p_json, "data");
    json_incref(p_data_json);
    retval.data = p_data_json;
    if (p_data_json != NULL)
    {
        retval.valid_fields |= COMPLETION_ITEM_FIELD_DATA;
    }


    if ((retval.valid_fields & COMPLETION_ITEM_FIELD_REQUIRED) != COMPLETION_ITEM_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for completion_item: Got 0x%x, expected 0x%x\n", retval.valid_fields, COMPLETION_ITEM_FIELD_REQUIRED);
    }
    return retval;
}
parameter_information_t decode_parameter_information(json_t * p_json)
{
    parameter_information_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_label_json = json_object_get(p_json, "label");
    if (json_is_string(p_label_json))
    {
        retval.label = decode_string(p_label_json);
        retval.valid_fields |= PARAMETER_INFORMATION_FIELD_LABEL;
    }
    json_decref(p_label_json);


    json_t * p_documentation_json = json_object_get(p_json, "documentation");
    if (json_is_object(p_documentation_json))
    {
        retval.documentation = decode_markup_content(p_documentation_json);
        retval.valid_fields |= PARAMETER_INFORMATION_FIELD_DOCUMENTATION;
    }
    json_decref(p_documentation_json);


    if ((retval.valid_fields & PARAMETER_INFORMATION_FIELD_REQUIRED) != PARAMETER_INFORMATION_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for parameter_information: Got 0x%x, expected 0x%x\n", retval.valid_fields, PARAMETER_INFORMATION_FIELD_REQUIRED);
    }
    return retval;
}
signature_information_t decode_signature_information(json_t * p_json)
{
    signature_information_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_label_json = json_object_get(p_json, "label");
    if (json_is_string(p_label_json))
    {
        retval.label = decode_string(p_label_json);
        retval.valid_fields |= SIGNATURE_INFORMATION_FIELD_LABEL;
    }
    json_decref(p_label_json);


    json_t * p_documentation_json = json_object_get(p_json, "documentation");
    if (json_is_object(p_documentation_json))
    {
        retval.documentation = decode_markup_content(p_documentation_json);
        retval.valid_fields |= SIGNATURE_INFORMATION_FIELD_DOCUMENTATION;
    }
    json_decref(p_documentation_json);


    json_t * p_parameters_json = json_object_get(p_json, "parameters");
    if (json_is_array(p_parameters_json))
    {
        retval.parameters_count = json_array_size(p_parameters_json);
        retval.p_parameters = malloc(sizeof(parameter_information_t) * retval.parameters_count);
        ASSERT(retval.p_parameters);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_parameters_json, index, p_it)
        {
            retval.p_parameters[index] = decode_parameter_information(p_it);
        }
        retval.valid_fields |= SIGNATURE_INFORMATION_FIELD_PARAMETERS;
    }
    json_decref(p_parameters_json);


    if ((retval.valid_fields & SIGNATURE_INFORMATION_FIELD_REQUIRED) != SIGNATURE_INFORMATION_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for signature_information: Got 0x%x, expected 0x%x\n", retval.valid_fields, SIGNATURE_INFORMATION_FIELD_REQUIRED);
    }
    return retval;
}
reference_context_t decode_reference_context(json_t * p_json)
{
    reference_context_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_include_declaration_json = json_object_get(p_json, "includeDeclaration");
    if (json_is_boolean(p_include_declaration_json))
    {
        retval.include_declaration = decode_boolean(p_include_declaration_json);
        retval.valid_fields |= REFERENCE_CONTEXT_FIELD_INCLUDE_DECLARATION;
    }
    json_decref(p_include_declaration_json);


    if ((retval.valid_fields & REFERENCE_CONTEXT_FIELD_REQUIRED) != REFERENCE_CONTEXT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for reference_context: Got 0x%x, expected 0x%x\n", retval.valid_fields, REFERENCE_CONTEXT_FIELD_REQUIRED);
    }
    return retval;
}
completion_list_t decode_completion_list(json_t * p_json)
{
    completion_list_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_is_incomplete_json = json_object_get(p_json, "isIncomplete");
    if (json_is_boolean(p_is_incomplete_json))
    {
        retval.is_incomplete = decode_boolean(p_is_incomplete_json);
        retval.valid_fields |= COMPLETION_LIST_FIELD_IS_INCOMPLETE;
    }
    json_decref(p_is_incomplete_json);


    json_t * p_items_json = json_object_get(p_json, "items");
    if (json_is_array(p_items_json))
    {
        retval.items_count = json_array_size(p_items_json);
        retval.p_items = malloc(sizeof(completion_item_t) * retval.items_count);
        ASSERT(retval.p_items);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_items_json, index, p_it)
        {
            retval.p_items[index] = decode_completion_item(p_it);
        }
        retval.valid_fields |= COMPLETION_LIST_FIELD_ITEMS;
    }
    json_decref(p_items_json);


    if ((retval.valid_fields & COMPLETION_LIST_FIELD_REQUIRED) != COMPLETION_LIST_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for completion_list: Got 0x%x, expected 0x%x\n", retval.valid_fields, COMPLETION_LIST_FIELD_REQUIRED);
    }
    return retval;
}
code_action_context_t decode_code_action_context(json_t * p_json)
{
    code_action_context_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_diagnostics_json = json_object_get(p_json, "diagnostics");
    if (json_is_array(p_diagnostics_json))
    {
        retval.diagnostics_count = json_array_size(p_diagnostics_json);
        retval.p_diagnostics = malloc(sizeof(diagnostic_t) * retval.diagnostics_count);
        ASSERT(retval.p_diagnostics);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_diagnostics_json, index, p_it)
        {
            retval.p_diagnostics[index] = decode_diagnostic(p_it);
        }
        retval.valid_fields |= CODE_ACTION_CONTEXT_FIELD_DIAGNOSTICS;
    }
    json_decref(p_diagnostics_json);


    if ((retval.valid_fields & CODE_ACTION_CONTEXT_FIELD_REQUIRED) != CODE_ACTION_CONTEXT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for code_action_context: Got 0x%x, expected 0x%x\n", retval.valid_fields, CODE_ACTION_CONTEXT_FIELD_REQUIRED);
    }
    return retval;
}


/* Server command parameter JSON decoders */
initialize_params_t decode_initialize_params(json_t * p_json)
{
    initialize_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_process_id_json = json_object_get(p_json, "processId");
    if (json_is_integer(p_process_id_json))
    {
        retval.process_id = decode_number(p_process_id_json);
        retval.valid_fields |= INITIALIZE_PARAMS_FIELD_PROCESS_ID;
    }
    json_decref(p_process_id_json);


    json_t * p_root_path_json = json_object_get(p_json, "rootPath");
    if (json_is_string(p_root_path_json))
    {
        retval.root_path = decode_string(p_root_path_json);
        retval.valid_fields |= INITIALIZE_PARAMS_FIELD_ROOT_PATH;
    }
    json_decref(p_root_path_json);


    json_t * p_root_uri_json = json_object_get(p_json, "rootUri");
    if (json_is_string(p_root_uri_json))
    {
        retval.root_uri = decode_uri(p_root_uri_json);
        retval.valid_fields |= INITIALIZE_PARAMS_FIELD_ROOT_URI;
    }
    json_decref(p_root_uri_json);


    json_t * p_capabilities_json = json_object_get(p_json, "capabilities");
    if (json_is_object(p_capabilities_json))
    {
        retval.capabilities = decode_client_capabilities(p_capabilities_json);
        retval.valid_fields |= INITIALIZE_PARAMS_FIELD_CAPABILITIES;
    }
    json_decref(p_capabilities_json);


    json_t * p_initialization_options_json = json_object_get(p_json, "initializationOptions");
    if (json_is_object(p_initialization_options_json))
    {
        retval.initialization_options = decode_initialization_options(p_initialization_options_json);
        retval.valid_fields |= INITIALIZE_PARAMS_FIELD_INITIALIZATION_OPTIONS;
    }
    json_decref(p_initialization_options_json);


    json_t * p_trace_json = json_object_get(p_json, "trace");
    if (json_is_string(p_trace_json))
    {
        retval.trace = decode_string(p_trace_json);
        retval.valid_fields |= INITIALIZE_PARAMS_FIELD_TRACE;
    }
    json_decref(p_trace_json);


    if ((retval.valid_fields & INITIALIZE_PARAMS_FIELD_REQUIRED) != INITIALIZE_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for initialize_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, INITIALIZE_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
reference_params_t decode_reference_params(json_t * p_json)
{
    reference_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_context_json = json_object_get(p_json, "context");
    if (json_is_object(p_context_json))
    {
        retval.context = decode_reference_context(p_context_json);
        retval.valid_fields |= REFERENCE_PARAMS_FIELD_CONTEXT;
    }
    json_decref(p_context_json);


    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= REFERENCE_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    json_t * p_position_json = json_object_get(p_json, "position");
    if (json_is_object(p_position_json))
    {
        retval.position = decode_position(p_position_json);
        retval.valid_fields |= REFERENCE_PARAMS_FIELD_POSITION;
    }
    json_decref(p_position_json);


    if ((retval.valid_fields & REFERENCE_PARAMS_FIELD_REQUIRED) != REFERENCE_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for reference_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, REFERENCE_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
document_symbol_params_t decode_document_symbol_params(json_t * p_json)
{
    document_symbol_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= DOCUMENT_SYMBOL_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    if ((retval.valid_fields & DOCUMENT_SYMBOL_PARAMS_FIELD_REQUIRED) != DOCUMENT_SYMBOL_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for document_symbol_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DOCUMENT_SYMBOL_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
workspace_symbol_params_t decode_workspace_symbol_params(json_t * p_json)
{
    workspace_symbol_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_query_json = json_object_get(p_json, "query");
    if (json_is_string(p_query_json))
    {
        retval.query = decode_string(p_query_json);
        retval.valid_fields |= WORKSPACE_SYMBOL_PARAMS_FIELD_QUERY;
    }
    json_decref(p_query_json);


    if ((retval.valid_fields & WORKSPACE_SYMBOL_PARAMS_FIELD_REQUIRED) != WORKSPACE_SYMBOL_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for workspace_symbol_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, WORKSPACE_SYMBOL_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
document_link_params_t decode_document_link_params(json_t * p_json)
{
    document_link_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= DOCUMENT_LINK_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    if ((retval.valid_fields & DOCUMENT_LINK_PARAMS_FIELD_REQUIRED) != DOCUMENT_LINK_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for document_link_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DOCUMENT_LINK_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
code_action_params_t decode_code_action_params(json_t * p_json)
{
    code_action_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= CODE_ACTION_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= CODE_ACTION_PARAMS_FIELD_RANGE;
    }
    json_decref(p_range_json);


    json_t * p_context_json = json_object_get(p_json, "context");
    if (json_is_object(p_context_json))
    {
        retval.context = decode_code_action_context(p_context_json);
        retval.valid_fields |= CODE_ACTION_PARAMS_FIELD_CONTEXT;
    }
    json_decref(p_context_json);


    if ((retval.valid_fields & CODE_ACTION_PARAMS_FIELD_REQUIRED) != CODE_ACTION_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for code_action_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, CODE_ACTION_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}


/* Client command parameter JSON decoders */
show_message_request_params_t decode_show_message_request_params(json_t * p_json)
{
    show_message_request_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_type_json = json_object_get(p_json, "type");
    if (json_is_object(p_type_json))
    {
        retval.type = decode_message_type(p_type_json);
        retval.valid_fields |= SHOW_MESSAGE_REQUEST_PARAMS_FIELD_TYPE;
    }
    json_decref(p_type_json);


    json_t * p_message_json = json_object_get(p_json, "message");
    if (json_is_string(p_message_json))
    {
        retval.message = decode_string(p_message_json);
        retval.valid_fields |= SHOW_MESSAGE_REQUEST_PARAMS_FIELD_MESSAGE;
    }
    json_decref(p_message_json);


    json_t * p_actions_json = json_object_get(p_json, "actions");
    if (json_is_array(p_actions_json))
    {
        retval.actions_count = json_array_size(p_actions_json);
        retval.p_actions = malloc(sizeof(message_action_item_t) * retval.actions_count);
        ASSERT(retval.p_actions);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_actions_json, index, p_it)
        {
            retval.p_actions[index] = decode_message_action_item(p_it);
        }
        retval.valid_fields |= SHOW_MESSAGE_REQUEST_PARAMS_FIELD_ACTIONS;
    }
    json_decref(p_actions_json);


    if ((retval.valid_fields & SHOW_MESSAGE_REQUEST_PARAMS_FIELD_REQUIRED) != SHOW_MESSAGE_REQUEST_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for show_message_request_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, SHOW_MESSAGE_REQUEST_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
signature_help_t decode_signature_help(json_t * p_json)
{
    signature_help_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_signatures_json = json_object_get(p_json, "signatures");
    if (json_is_array(p_signatures_json))
    {
        retval.signatures_count = json_array_size(p_signatures_json);
        retval.p_signatures = malloc(sizeof(signature_information_t) * retval.signatures_count);
        ASSERT(retval.p_signatures);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_signatures_json, index, p_it)
        {
            retval.p_signatures[index] = decode_signature_information(p_it);
        }
        retval.valid_fields |= SIGNATURE_HELP_FIELD_SIGNATURES;
    }
    json_decref(p_signatures_json);


    json_t * p_active_signature_json = json_object_get(p_json, "activeSignature");
    if (json_is_integer(p_active_signature_json))
    {
        retval.active_signature = decode_number(p_active_signature_json);
        retval.valid_fields |= SIGNATURE_HELP_FIELD_ACTIVE_SIGNATURE;
    }
    json_decref(p_active_signature_json);


    json_t * p_active_parameter_json = json_object_get(p_json, "activeParameter");
    if (json_is_integer(p_active_parameter_json))
    {
        retval.active_parameter = decode_number(p_active_parameter_json);
        retval.valid_fields |= SIGNATURE_HELP_FIELD_ACTIVE_PARAMETER;
    }
    json_decref(p_active_parameter_json);


    if ((retval.valid_fields & SIGNATURE_HELP_FIELD_REQUIRED) != SIGNATURE_HELP_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for signature_help: Got 0x%x, expected 0x%x\n", retval.valid_fields, SIGNATURE_HELP_FIELD_REQUIRED);
    }
    return retval;
}


/* Command response parameter JSON decoders */
initialize_result_t decode_initialize_result(json_t * p_json)
{
    initialize_result_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_capabilities_json = json_object_get(p_json, "capabilities");
    if (json_is_object(p_capabilities_json))
    {
        retval.capabilities = decode_server_capabilities(p_capabilities_json);
        retval.valid_fields |= INITIALIZE_RESULT_FIELD_CAPABILITIES;
    }
    json_decref(p_capabilities_json);


    if ((retval.valid_fields & INITIALIZE_RESULT_FIELD_REQUIRED) != INITIALIZE_RESULT_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for initialize_result: Got 0x%x, expected 0x%x\n", retval.valid_fields, INITIALIZE_RESULT_FIELD_REQUIRED);
    }
    return retval;
}
symbol_information_t decode_symbol_information(json_t * p_json)
{
    symbol_information_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_name_json = json_object_get(p_json, "name");
    if (json_is_string(p_name_json))
    {
        retval.name = decode_string(p_name_json);
        retval.valid_fields |= SYMBOL_INFORMATION_FIELD_NAME;
    }
    json_decref(p_name_json);


    json_t * p_kind_json = json_object_get(p_json, "kind");
    if (json_is_object(p_kind_json))
    {
        retval.kind = decode_symbol_kind(p_kind_json);
        retval.valid_fields |= SYMBOL_INFORMATION_FIELD_KIND;
    }
    json_decref(p_kind_json);


    json_t * p_location_json = json_object_get(p_json, "location");
    if (json_is_object(p_location_json))
    {
        retval.location = decode_location(p_location_json);
        retval.valid_fields |= SYMBOL_INFORMATION_FIELD_LOCATION;
    }
    json_decref(p_location_json);


    json_t * p_container_name_json = json_object_get(p_json, "containerName");
    if (json_is_string(p_container_name_json))
    {
        retval.container_name = decode_string(p_container_name_json);
        retval.valid_fields |= SYMBOL_INFORMATION_FIELD_CONTAINER_NAME;
    }
    json_decref(p_container_name_json);


    if ((retval.valid_fields & SYMBOL_INFORMATION_FIELD_REQUIRED) != SYMBOL_INFORMATION_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for symbol_information: Got 0x%x, expected 0x%x\n", retval.valid_fields, SYMBOL_INFORMATION_FIELD_REQUIRED);
    }
    return retval;
}
document_link_t decode_document_link(json_t * p_json)
{
    document_link_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= DOCUMENT_LINK_FIELD_RANGE;
    }
    json_decref(p_range_json);


    json_t * p_target_json = json_object_get(p_json, "target");
    if (json_is_string(p_target_json))
    {
        retval.target = decode_uri(p_target_json);
        retval.valid_fields |= DOCUMENT_LINK_FIELD_TARGET;
    }
    json_decref(p_target_json);


    if ((retval.valid_fields & DOCUMENT_LINK_FIELD_REQUIRED) != DOCUMENT_LINK_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for document_link: Got 0x%x, expected 0x%x\n", retval.valid_fields, DOCUMENT_LINK_FIELD_REQUIRED);
    }
    return retval;
}
hover_t decode_hover(json_t * p_json)
{
    hover_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_contents_json = json_object_get(p_json, "contents");
    if (json_is_object(p_contents_json))
    {
        retval.contents = decode_markup_content(p_contents_json);
        retval.valid_fields |= HOVER_FIELD_CONTENTS;
    }
    json_decref(p_contents_json);


    json_t * p_range_json = json_object_get(p_json, "range");
    if (json_is_object(p_range_json))
    {
        retval.range = decode_range(p_range_json);
        retval.valid_fields |= HOVER_FIELD_RANGE;
    }
    json_decref(p_range_json);


    if ((retval.valid_fields & HOVER_FIELD_REQUIRED) != HOVER_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for hover: Got 0x%x, expected 0x%x\n", retval.valid_fields, HOVER_FIELD_REQUIRED);
    }
    return retval;
}


/* Client notification parameter JSON decoders */
show_message_params_t decode_show_message_params(json_t * p_json)
{
    show_message_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_type_json = json_object_get(p_json, "type");
    if (json_is_object(p_type_json))
    {
        retval.type = decode_message_type(p_type_json);
        retval.valid_fields |= SHOW_MESSAGE_PARAMS_FIELD_TYPE;
    }
    json_decref(p_type_json);


    json_t * p_message_json = json_object_get(p_json, "message");
    if (json_is_string(p_message_json))
    {
        retval.message = decode_string(p_message_json);
        retval.valid_fields |= SHOW_MESSAGE_PARAMS_FIELD_MESSAGE;
    }
    json_decref(p_message_json);


    if ((retval.valid_fields & SHOW_MESSAGE_PARAMS_FIELD_REQUIRED) != SHOW_MESSAGE_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for show_message_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, SHOW_MESSAGE_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
log_message_params_t decode_log_message_params(json_t * p_json)
{
    log_message_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_type_json = json_object_get(p_json, "type");
    if (json_is_object(p_type_json))
    {
        retval.type = decode_message_type(p_type_json);
        retval.valid_fields |= LOG_MESSAGE_PARAMS_FIELD_TYPE;
    }
    json_decref(p_type_json);


    json_t * p_message_json = json_object_get(p_json, "message");
    if (json_is_string(p_message_json))
    {
        retval.message = decode_string(p_message_json);
        retval.valid_fields |= LOG_MESSAGE_PARAMS_FIELD_MESSAGE;
    }
    json_decref(p_message_json);


    if ((retval.valid_fields & LOG_MESSAGE_PARAMS_FIELD_REQUIRED) != LOG_MESSAGE_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for log_message_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, LOG_MESSAGE_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
publish_diagnostics_params_t decode_publish_diagnostics_params(json_t * p_json)
{
    publish_diagnostics_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_uri_json = json_object_get(p_json, "uri");
    if (json_is_string(p_uri_json))
    {
        retval.uri = decode_uri(p_uri_json);
        retval.valid_fields |= PUBLISH_DIAGNOSTICS_PARAMS_FIELD_URI;
    }
    json_decref(p_uri_json);


    json_t * p_diagnostics_json = json_object_get(p_json, "diagnostics");
    if (json_is_array(p_diagnostics_json))
    {
        retval.diagnostics_count = json_array_size(p_diagnostics_json);
        retval.p_diagnostics = malloc(sizeof(diagnostic_t) * retval.diagnostics_count);
        ASSERT(retval.p_diagnostics);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_diagnostics_json, index, p_it)
        {
            retval.p_diagnostics[index] = decode_diagnostic(p_it);
        }
        retval.valid_fields |= PUBLISH_DIAGNOSTICS_PARAMS_FIELD_DIAGNOSTICS;
    }
    json_decref(p_diagnostics_json);


    if ((retval.valid_fields & PUBLISH_DIAGNOSTICS_PARAMS_FIELD_REQUIRED) != PUBLISH_DIAGNOSTICS_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for publish_diagnostics_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, PUBLISH_DIAGNOSTICS_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}


/* Server notification parameter JSON decoders */
did_open_text_document_params_t decode_did_open_text_document_params(json_t * p_json)
{
    did_open_text_document_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_item(p_text_document_json);
        retval.valid_fields |= DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    if ((retval.valid_fields & DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED) != DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for did_open_text_document_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
did_change_text_document_params_t decode_did_change_text_document_params(json_t * p_json)
{
    did_change_text_document_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_versioned_text_document_identifier(p_text_document_json);
        retval.valid_fields |= DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    json_t * p_content_changes_json = json_object_get(p_json, "contentChanges");
    if (json_is_array(p_content_changes_json))
    {
        retval.content_changes_count = json_array_size(p_content_changes_json);
        retval.p_content_changes = malloc(sizeof(text_document_content_change_event_t) * retval.content_changes_count);
        ASSERT(retval.p_content_changes);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_content_changes_json, index, p_it)
        {
            retval.p_content_changes[index] = decode_text_document_content_change_event(p_it);
        }
        retval.valid_fields |= DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_CONTENT_CHANGES;
    }
    json_decref(p_content_changes_json);


    if ((retval.valid_fields & DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED) != DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for did_change_text_document_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
did_save_text_document_params_t decode_did_save_text_document_params(json_t * p_json)
{
    did_save_text_document_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    json_t * p_text_json = json_object_get(p_json, "text");
    if (json_is_string(p_text_json))
    {
        retval.text = decode_string(p_text_json);
        retval.valid_fields |= DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT;
    }
    json_decref(p_text_json);


    if ((retval.valid_fields & DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED) != DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for did_save_text_document_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
did_close_text_document_params_t decode_did_close_text_document_params(json_t * p_json)
{
    did_close_text_document_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_text_document_json = json_object_get(p_json, "textDocument");
    if (json_is_object(p_text_document_json))
    {
        retval.text_document = decode_text_document_identifier(p_text_document_json);
        retval.valid_fields |= DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT;
    }
    json_decref(p_text_document_json);


    if ((retval.valid_fields & DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED) != DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for did_close_text_document_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}
did_change_watched_files_params_t decode_did_change_watched_files_params(json_t * p_json)
{
    did_change_watched_files_params_t retval;
    memset(&retval, 0, sizeof(retval));

    json_t * p_changes_json = json_object_get(p_json, "changes");
    if (json_is_array(p_changes_json))
    {
        retval.changes_count = json_array_size(p_changes_json);
        retval.p_changes = malloc(sizeof(file_event_t) * retval.changes_count);
        ASSERT(retval.p_changes);
        json_t * p_it;
        uint32_t index;
        json_array_foreach(p_changes_json, index, p_it)
        {
            retval.p_changes[index] = decode_file_event(p_it);
        }
        retval.valid_fields |= DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_CHANGES;
    }
    json_decref(p_changes_json);


    if ((retval.valid_fields & DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_REQUIRED) != DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_REQUIRED)
    {
        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;
        LOG("Missing required parameters for did_change_watched_files_params: Got 0x%x, expected 0x%x\n", retval.valid_fields, DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_REQUIRED);
    }
    return retval;
}

/*******************************************************************************
 * Freers
 ******************************************************************************/
/* Common parameter structure freers */
void free_initialization_options(initialization_options_t value)
{
    for (uint32_t i = 0; i < value.flags_count; ++i)
    {
        free_string(value.p_flags[i]);
    }
    free(value.p_flags);
    for (uint32_t i = 0; i < value.compilation_database_count; ++i)
    {
        free_string(value.p_compilation_database[i]);
    }
    free(value.p_compilation_database);
}

void free_workspace_client_capabilities(workspace_client_capabilities_t value)
{
}

void free_text_document_client_capabilities(text_document_client_capabilities_t value)
{
            for (uint32_t i = 0; i < value.signature_help.signature_information.documentation_format_count; ++i)
    {
        free_string(value.signature_help.signature_information.p_documentation_format[i]);
    }
    free(value.signature_help.signature_information.p_documentation_format);
    
    
    
    
    
    
    
    
    
    
}

void free_client_capabilities(client_capabilities_t value)
{
    free_workspace_client_capabilities(value.workspace);
    free_text_document_client_capabilities(value.text_document);
    json_decref(value.experimenal);
}

void free_completion_options(completion_options_t value)
{
    for (uint32_t i = 0; i < value.trigger_characters_count; ++i)
    {
        free_string(value.p_trigger_characters[i]);
    }
    free(value.p_trigger_characters);
}

void free_signature_help_options(signature_help_options_t value)
{
    for (uint32_t i = 0; i < value.trigger_characters_count; ++i)
    {
        free_string(value.p_trigger_characters[i]);
    }
    free(value.p_trigger_characters);
}

void free_code_lens_options(code_lens_options_t value)
{
}

void free_save_options(save_options_t value)
{
}

void free_text_document_sync_options(text_document_sync_options_t value)
{
    free_save_options(value.save);
}

void free_server_capabilities(server_capabilities_t value)
{
    free_text_document_sync_options(value.text_document_sync);
    free_completion_options(value.completion_provider);
    free_signature_help_options(value.signature_help_provider);
    free_code_lens_options(value.code_lens_provider);
}

void free_markup_content(markup_content_t value)
{
    free_string(value.kind);
    free_string(value.value);
}

void free_marked_string(marked_string_t value)
{
    free_string(value.language);
    free_string(value.value);
}

void free_message_action_item(message_action_item_t value)
{
    free_string(value.title);
}

void free_position(position_t value)
{
}

void free_range(range_t value)
{
    free_position(value.start);
    free_position(value.end);
}

void free_text_edit(text_edit_t value)
{
    free_range(value.range);
    free_string(value.new_text);
}

void free_diagnostic(diagnostic_t value)
{
    free_range(value.range);
    free_string(value.source);
    free_string(value.message);
}

void free_location(location_t value)
{
    free_uri(value.uri);
    free_range(value.range);
}

void free_text_document_item(text_document_item_t value)
{
    free_uri(value.uri);
    free_string(value.language_id);
    free_string(value.text);
}

void free_text_document_content_change_event(text_document_content_change_event_t value)
{
    free_range(value.range);
    free_string(value.text);
}

void free_text_document_identifier(text_document_identifier_t value)
{
    free_uri(value.uri);
}

void free_versioned_text_document_identifier(versioned_text_document_identifier_t value)
{
    free_uri(value.uri);
}

void free_file_event(file_event_t value)
{
    free_uri(value.uri);
}

void free_text_document_position_params(text_document_position_params_t value)
{
    free_text_document_identifier(value.text_document);
    free_position(value.position);
}

void free_command(command_t value)
{
    free_string(value.title);
    free_string(value.command);
    json_decref(value.arguments);
}

void free_completion_item(completion_item_t value)
{
    free_string(value.label);
    free_string(value.detail);
    free_markup_content(value.documentation);
    free_string(value.sort_text);
    free_string(value.filter_text);
    free_string(value.insert_text);
    free_text_edit(value.text_edit);
    for (uint32_t i = 0; i < value.additional_text_edits_count; ++i)
    {
        free_text_edit(value.p_additional_text_edits[i]);
    }
    free(value.p_additional_text_edits);
    for (uint32_t i = 0; i < value.commit_characters_count; ++i)
    {
        free_string(value.p_commit_characters[i]);
    }
    free(value.p_commit_characters);
    free_command(value.command);
    json_decref(value.data);
}

void free_parameter_information(parameter_information_t value)
{
    free_string(value.label);
    free_markup_content(value.documentation);
}

void free_signature_information(signature_information_t value)
{
    free_string(value.label);
    free_markup_content(value.documentation);
    for (uint32_t i = 0; i < value.parameters_count; ++i)
    {
        free_parameter_information(value.p_parameters[i]);
    }
    free(value.p_parameters);
}

void free_reference_context(reference_context_t value)
{
}

void free_completion_list(completion_list_t value)
{
    for (uint32_t i = 0; i < value.items_count; ++i)
    {
        free_completion_item(value.p_items[i]);
    }
    free(value.p_items);
}

void free_code_action_context(code_action_context_t value)
{
    for (uint32_t i = 0; i < value.diagnostics_count; ++i)
    {
        free_diagnostic(value.p_diagnostics[i]);
    }
    free(value.p_diagnostics);
}


/* Server command parameter structure freers */
void free_initialize_params(initialize_params_t value)
{
    free_string(value.root_path);
    free_uri(value.root_uri);
    free_client_capabilities(value.capabilities);
    free_initialization_options(value.initialization_options);
    free_string(value.trace);
}

void free_reference_params(reference_params_t value)
{
    free_reference_context(value.context);
    free_text_document_identifier(value.text_document);
    free_position(value.position);
}

void free_document_symbol_params(document_symbol_params_t value)
{
    free_text_document_identifier(value.text_document);
}

void free_workspace_symbol_params(workspace_symbol_params_t value)
{
    free_string(value.query);
}

void free_document_link_params(document_link_params_t value)
{
    free_text_document_identifier(value.text_document);
}

void free_code_action_params(code_action_params_t value)
{
    free_text_document_identifier(value.text_document);
    free_range(value.range);
    free_code_action_context(value.context);
}


/* Client command parameter structure freers */
void free_show_message_request_params(show_message_request_params_t value)
{
    free_string(value.message);
    for (uint32_t i = 0; i < value.actions_count; ++i)
    {
        free_message_action_item(value.p_actions[i]);
    }
    free(value.p_actions);
}

void free_signature_help(signature_help_t value)
{
    for (uint32_t i = 0; i < value.signatures_count; ++i)
    {
        free_signature_information(value.p_signatures[i]);
    }
    free(value.p_signatures);
}


/* Command response parameter structure freers */
void free_initialize_result(initialize_result_t value)
{
    free_server_capabilities(value.capabilities);
}

void free_symbol_information(symbol_information_t value)
{
    free_string(value.name);
    free_location(value.location);
    free_string(value.container_name);
}

void free_document_link(document_link_t value)
{
    free_range(value.range);
    free_uri(value.target);
}

void free_hover(hover_t value)
{
    free_markup_content(value.contents);
    free_range(value.range);
}


/* Client notification parameter structure freers */
void free_show_message_params(show_message_params_t value)
{
    free_string(value.message);
}

void free_log_message_params(log_message_params_t value)
{
    free_string(value.message);
}

void free_publish_diagnostics_params(publish_diagnostics_params_t value)
{
    free_uri(value.uri);
    for (uint32_t i = 0; i < value.diagnostics_count; ++i)
    {
        free_diagnostic(value.p_diagnostics[i]);
    }
    free(value.p_diagnostics);
}


/* Server notification parameter structure freers */
void free_did_open_text_document_params(did_open_text_document_params_t value)
{
    free_text_document_item(value.text_document);
}

void free_did_change_text_document_params(did_change_text_document_params_t value)
{
    free_versioned_text_document_identifier(value.text_document);
    for (uint32_t i = 0; i < value.content_changes_count; ++i)
    {
        free_text_document_content_change_event(value.p_content_changes[i]);
    }
    free(value.p_content_changes);
}

void free_did_save_text_document_params(did_save_text_document_params_t value)
{
    free_text_document_identifier(value.text_document);
    free_string(value.text);
}

void free_did_close_text_document_params(did_close_text_document_params_t value)
{
    free_text_document_identifier(value.text_document);
}

void free_did_change_watched_files_params(did_change_watched_files_params_t value)
{
    for (uint32_t i = 0; i < value.changes_count; ++i)
    {
        free_file_event(value.p_changes[i]);
    }
    free(value.p_changes);
}
decoder_error_t decoder_error(void)
{
    decoder_error_t e = m_error;
    m_error = DECODER_ERROR_NONE;
    return e;
}
