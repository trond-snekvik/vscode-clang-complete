#pragma once
#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define MARKUP_KIND_MARKUP "markdown"
#define MARKUP_KIND_PLAINTEXT "plaintext"
/*******************************************************************************
 * Field enumerators
 ******************************************************************************/
/* Common parameter fields */
typedef enum
{
    COMPILATION_DATABASE_PARAMS_FIELD_PATH                 = (1 << 0),
    COMPILATION_DATABASE_PARAMS_FIELD_ADDITIONAL_ARGUMENTS = (1 << 1),

    COMPILATION_DATABASE_PARAMS_FIELD_ALL = (0x3),
    COMPILATION_DATABASE_PARAMS_FIELD_REQUIRED = (COMPILATION_DATABASE_PARAMS_FIELD_PATH)
} compilation_database_params_fields_t;

typedef enum
{
    INITIALIZATION_OPTIONS_FIELD_FLAGS                = (1 << 0),
    INITIALIZATION_OPTIONS_FIELD_COMPILATION_DATABASE = (1 << 1),

    INITIALIZATION_OPTIONS_FIELD_ALL = (0x3),
} initialization_options_fields_t;

typedef enum
{
    WORKSPACE_CLIENT_CAPABILITIES_FIELD_APPLY_EDIT               = (1 << 0),
    WORKSPACE_CLIENT_CAPABILITIES_FIELD_WORKSPACE_EDIT           = (1 << 1),
    WORKSPACE_CLIENT_CAPABILITIES_FIELD_DID_CHANGE_CONFIGURATION = (1 << 2),
    WORKSPACE_CLIENT_CAPABILITIES_FIELD_DID_CHANGE_WATCHED_FILES = (1 << 3),
    WORKSPACE_CLIENT_CAPABILITIES_FIELD_SYMBOL                   = (1 << 4),
    WORKSPACE_CLIENT_CAPABILITIES_FIELD_EXECUTE_COMMAND          = (1 << 5),

    WORKSPACE_CLIENT_CAPABILITIES_FIELD_ALL = (0x3f),
} workspace_client_capabilities_fields_t;

typedef enum
{
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_SYNCHRONIZATION    = (1 << 0),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_COMPLETION         = (1 << 1),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_HOVER              = (1 << 2),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_SIGNATURE_HELP     = (1 << 3),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_REFERENCES         = (1 << 4),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_HIGHLIGHT = (1 << 5),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_SYMBOL    = (1 << 6),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_FORMATTING         = (1 << 7),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_ON_TYPE_FORMATTING = (1 << 8),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DEFINITION         = (1 << 9),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_CODE_ACTION        = (1 << 10),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_CODE_LENS          = (1 << 11),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_DOCUMENT_LINK      = (1 << 12),
    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_RENAME             = (1 << 13),

    TEXT_DOCUMENT_CLIENT_CAPABILITIES_FIELD_ALL = (0x3fff),
} text_document_client_capabilities_fields_t;

typedef enum
{
    CLIENT_CAPABILITIES_FIELD_WORKSPACE     = (1 << 0),
    CLIENT_CAPABILITIES_FIELD_TEXT_DOCUMENT = (1 << 1),
    CLIENT_CAPABILITIES_FIELD_EXPERIMENTAL  = (1 << 2),

    CLIENT_CAPABILITIES_FIELD_ALL = (0x7),
} client_capabilities_fields_t;

typedef enum
{
    COMPLETION_OPTIONS_FIELD_RESOLVE_PROVIDER   = (1 << 0),
    COMPLETION_OPTIONS_FIELD_TRIGGER_CHARACTERS = (1 << 1),

    COMPLETION_OPTIONS_FIELD_ALL = (0x3),
} completion_options_fields_t;

typedef enum
{
    SIGNATURE_HELP_OPTIONS_FIELD_TRIGGER_CHARACTERS = (1 << 0),

    SIGNATURE_HELP_OPTIONS_FIELD_ALL = (0x1),
} signature_help_options_fields_t;

typedef enum
{
    CODE_LENS_OPTIONS_FIELD_RESOLVE_PROVIDER = (1 << 0),

    CODE_LENS_OPTIONS_FIELD_ALL = (0x1),
} code_lens_options_fields_t;

typedef enum
{
    SAVE_OPTIONS_FIELD_INCLUDE_TEXT = (1 << 0),

    SAVE_OPTIONS_FIELD_ALL = (0x1),
} save_options_fields_t;

typedef enum
{
    TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_OPEN_CLOSE           = (1 << 0),
    TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_CHANGE               = (1 << 1),
    TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_WILL_SAVE            = (1 << 2),
    TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_WILL_SAVE_WAIT_UNTIL = (1 << 3),
    TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_SAVE                 = (1 << 4),

    TEXT_DOCUMENT_SYNC_OPTIONS_FIELD_ALL = (0x1f),
} text_document_sync_options_fields_t;

typedef enum
{
    SERVER_CAPABILITIES_FIELD_TEXT_DOCUMENT_SYNC           = (1 << 0),
    SERVER_CAPABILITIES_FIELD_HOVER_PROVIDER               = (1 << 1),
    SERVER_CAPABILITIES_FIELD_COMPLETION_PROVIDER          = (1 << 2),
    SERVER_CAPABILITIES_FIELD_SIGNATURE_HELP_PROVIDER      = (1 << 3),
    SERVER_CAPABILITIES_FIELD_DEFINITION_PROVIDER          = (1 << 4),
    SERVER_CAPABILITIES_FIELD_REFERENCES_PROVIDER          = (1 << 5),
    SERVER_CAPABILITIES_FIELD_DOCUMENT_HIGHLIGHT_PROVIDER  = (1 << 6),
    SERVER_CAPABILITIES_FIELD_DOCUMENT_SYMBOL_PROVIDER     = (1 << 7),
    SERVER_CAPABILITIES_FIELD_WORKSPACE_SYMBOL_PROVIDER    = (1 << 8),
    SERVER_CAPABILITIES_FIELD_CODE_ACTION_PROVIDER         = (1 << 9),
    SERVER_CAPABILITIES_FIELD_CODE_LENS_PROVIDER           = (1 << 10),
    SERVER_CAPABILITIES_FIELD_DOCUMENT_FORMATTING_PROVIDER = (1 << 11),

    SERVER_CAPABILITIES_FIELD_ALL = (0xfff),
} server_capabilities_fields_t;

typedef enum
{
    MARKUP_CONTENT_FIELD_KIND  = (1 << 0),
    MARKUP_CONTENT_FIELD_VALUE = (1 << 1),

    MARKUP_CONTENT_FIELD_ALL = (0x3),
    MARKUP_CONTENT_FIELD_REQUIRED = (MARKUP_CONTENT_FIELD_ALL)
} markup_content_fields_t;

typedef enum
{
    MARKED_STRING_FIELD_LANGUAGE = (1 << 0),
    MARKED_STRING_FIELD_VALUE    = (1 << 1),

    MARKED_STRING_FIELD_ALL = (0x3),
    MARKED_STRING_FIELD_REQUIRED = (MARKED_STRING_FIELD_ALL)
} marked_string_fields_t;

typedef enum
{
    MESSAGE_ACTION_ITEM_FIELD_TITLE = (1 << 0),

    MESSAGE_ACTION_ITEM_FIELD_ALL = (0x1),
    MESSAGE_ACTION_ITEM_FIELD_REQUIRED = (MESSAGE_ACTION_ITEM_FIELD_ALL)
} message_action_item_fields_t;

typedef enum
{
    POSITION_FIELD_LINE      = (1 << 0),
    POSITION_FIELD_CHARACTER = (1 << 1),

    POSITION_FIELD_ALL = (0x3),
    POSITION_FIELD_REQUIRED = (POSITION_FIELD_ALL)
} position_fields_t;

typedef enum
{
    RANGE_FIELD_START = (1 << 0),
    RANGE_FIELD_END   = (1 << 1),

    RANGE_FIELD_ALL = (0x3),
    RANGE_FIELD_REQUIRED = (RANGE_FIELD_ALL)
} range_fields_t;

typedef enum
{
    TEXT_EDIT_FIELD_RANGE    = (1 << 0),
    TEXT_EDIT_FIELD_NEW_TEXT = (1 << 1),

    TEXT_EDIT_FIELD_ALL = (0x3),
    TEXT_EDIT_FIELD_REQUIRED = (TEXT_EDIT_FIELD_ALL)
} text_edit_fields_t;

typedef enum
{
    LOCATION_FIELD_URI   = (1 << 0),
    LOCATION_FIELD_RANGE = (1 << 1),

    LOCATION_FIELD_ALL = (0x3),
    LOCATION_FIELD_REQUIRED = (LOCATION_FIELD_ALL)
} location_fields_t;

typedef enum
{
    DIAGNOSTIC_RELATED_INFORMATION_FIELD_LOCATION = (1 << 0),
    DIAGNOSTIC_RELATED_INFORMATION_FIELD_MESSAGE  = (1 << 1),

    DIAGNOSTIC_RELATED_INFORMATION_FIELD_ALL = (0x3),
    DIAGNOSTIC_RELATED_INFORMATION_FIELD_REQUIRED = (DIAGNOSTIC_RELATED_INFORMATION_FIELD_ALL)
} diagnostic_related_information_fields_t;

typedef enum
{
    DIAGNOSTIC_FIELD_RANGE               = (1 << 0),
    DIAGNOSTIC_FIELD_SEVERITY            = (1 << 1),
    DIAGNOSTIC_FIELD_CODE                = (1 << 2),
    DIAGNOSTIC_FIELD_SOURCE              = (1 << 3),
    DIAGNOSTIC_FIELD_MESSAGE             = (1 << 4),
    DIAGNOSTIC_FIELD_RELATED_INFORMATION = (1 << 5),

    DIAGNOSTIC_FIELD_ALL = (0x3f),
    DIAGNOSTIC_FIELD_REQUIRED = (DIAGNOSTIC_FIELD_RANGE | DIAGNOSTIC_FIELD_MESSAGE)
} diagnostic_fields_t;

typedef enum
{
    TEXT_DOCUMENT_ITEM_FIELD_URI         = (1 << 0),
    TEXT_DOCUMENT_ITEM_FIELD_LANGUAGE_ID = (1 << 1),
    TEXT_DOCUMENT_ITEM_FIELD_VERSION     = (1 << 2),
    TEXT_DOCUMENT_ITEM_FIELD_TEXT        = (1 << 3),

    TEXT_DOCUMENT_ITEM_FIELD_ALL = (0xf),
    TEXT_DOCUMENT_ITEM_FIELD_REQUIRED = (TEXT_DOCUMENT_ITEM_FIELD_ALL)
} text_document_item_fields_t;

typedef enum
{
    TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_RANGE        = (1 << 0),
    TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_RANGE_LENGTH = (1 << 1),
    TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_TEXT         = (1 << 2),

    TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_ALL = (0x7),
    TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_REQUIRED = (TEXT_DOCUMENT_CONTENT_CHANGE_EVENT_FIELD_TEXT)
} text_document_content_change_event_fields_t;

typedef enum
{
    TEXT_DOCUMENT_IDENTIFIER_FIELD_URI = (1 << 0),

    TEXT_DOCUMENT_IDENTIFIER_FIELD_ALL = (0x1),
    TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED = (TEXT_DOCUMENT_IDENTIFIER_FIELD_ALL)
} text_document_identifier_fields_t;

typedef enum
{
    VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_URI     = (1 << 0),
    VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_VERSION = (1 << 1),

    VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_ALL = (0x3),
    VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_REQUIRED = (VERSIONED_TEXT_DOCUMENT_IDENTIFIER_FIELD_ALL)
} versioned_text_document_identifier_fields_t;

typedef enum
{
    FILE_EVENT_FIELD_URI  = (1 << 0),
    FILE_EVENT_FIELD_TYPE = (1 << 1),

    FILE_EVENT_FIELD_ALL = (0x3),
    FILE_EVENT_FIELD_REQUIRED = (FILE_EVENT_FIELD_ALL)
} file_event_fields_t;

typedef enum
{
    TEXT_DOCUMENT_POSITION_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),
    TEXT_DOCUMENT_POSITION_PARAMS_FIELD_POSITION      = (1 << 1),

    TEXT_DOCUMENT_POSITION_PARAMS_FIELD_ALL = (0x3),
    TEXT_DOCUMENT_POSITION_PARAMS_FIELD_REQUIRED = (TEXT_DOCUMENT_POSITION_PARAMS_FIELD_ALL)
} text_document_position_params_fields_t;

typedef enum
{
    COMMAND_FIELD_TITLE     = (1 << 0),
    COMMAND_FIELD_COMMAND   = (1 << 1),
    COMMAND_FIELD_ARGUMENTS = (1 << 2),

    COMMAND_FIELD_ALL = (0x7),
    COMMAND_FIELD_REQUIRED = (COMMAND_FIELD_TITLE | COMMAND_FIELD_COMMAND)
} command_fields_t;

typedef enum
{
    COMPLETION_ITEM_FIELD_LABEL                 = (1 << 0),
    COMPLETION_ITEM_FIELD_KIND                  = (1 << 1),
    COMPLETION_ITEM_FIELD_DETAIL                = (1 << 2),
    COMPLETION_ITEM_FIELD_DOCUMENTATION         = (1 << 3),
    COMPLETION_ITEM_FIELD_SORT_TEXT             = (1 << 4),
    COMPLETION_ITEM_FIELD_FILTER_TEXT           = (1 << 5),
    COMPLETION_ITEM_FIELD_INSERT_TEXT           = (1 << 6),
    COMPLETION_ITEM_FIELD_INSERT_TEXT_FORMAT    = (1 << 7),
    COMPLETION_ITEM_FIELD_TEXT_EDIT             = (1 << 8),
    COMPLETION_ITEM_FIELD_ADDITIONAL_TEXT_EDITS = (1 << 9),
    COMPLETION_ITEM_FIELD_COMMIT_CHARACTERS     = (1 << 10),
    COMPLETION_ITEM_FIELD_COMMAND               = (1 << 11),
    COMPLETION_ITEM_FIELD_DATA                  = (1 << 12),

    COMPLETION_ITEM_FIELD_ALL = (0x1fff),
    COMPLETION_ITEM_FIELD_REQUIRED = (COMPLETION_ITEM_FIELD_LABEL)
} completion_item_fields_t;

typedef enum
{
    PARAMETER_INFORMATION_FIELD_LABEL         = (1 << 0),
    PARAMETER_INFORMATION_FIELD_DOCUMENTATION = (1 << 1),

    PARAMETER_INFORMATION_FIELD_ALL = (0x3),
    PARAMETER_INFORMATION_FIELD_REQUIRED = (PARAMETER_INFORMATION_FIELD_LABEL)
} parameter_information_fields_t;

typedef enum
{
    SIGNATURE_INFORMATION_FIELD_LABEL         = (1 << 0),
    SIGNATURE_INFORMATION_FIELD_DOCUMENTATION = (1 << 1),
    SIGNATURE_INFORMATION_FIELD_PARAMETERS    = (1 << 2),

    SIGNATURE_INFORMATION_FIELD_ALL = (0x7),
    SIGNATURE_INFORMATION_FIELD_REQUIRED = (SIGNATURE_INFORMATION_FIELD_LABEL)
} signature_information_fields_t;

typedef enum
{
    REFERENCE_CONTEXT_FIELD_INCLUDE_DECLARATION = (1 << 0),

    REFERENCE_CONTEXT_FIELD_ALL = (0x1),
    REFERENCE_CONTEXT_FIELD_REQUIRED = (REFERENCE_CONTEXT_FIELD_ALL)
} reference_context_fields_t;

typedef enum
{
    COMPLETION_LIST_FIELD_IS_INCOMPLETE = (1 << 0),
    COMPLETION_LIST_FIELD_ITEMS         = (1 << 1),

    COMPLETION_LIST_FIELD_ALL = (0x3),
    COMPLETION_LIST_FIELD_REQUIRED = (COMPLETION_LIST_FIELD_ALL)
} completion_list_fields_t;

typedef enum
{
    CODE_ACTION_CONTEXT_FIELD_DIAGNOSTICS = (1 << 0),

    CODE_ACTION_CONTEXT_FIELD_ALL = (0x1),
    CODE_ACTION_CONTEXT_FIELD_REQUIRED = (CODE_ACTION_CONTEXT_FIELD_ALL)
} code_action_context_fields_t;



/* Server command parameter fields */
typedef enum
{
    INITIALIZE_PARAMS_FIELD_PROCESS_ID             = (1 << 0),
    INITIALIZE_PARAMS_FIELD_ROOT_PATH              = (1 << 1),
    INITIALIZE_PARAMS_FIELD_ROOT_URI               = (1 << 2),
    INITIALIZE_PARAMS_FIELD_CAPABILITIES           = (1 << 3),
    INITIALIZE_PARAMS_FIELD_INITIALIZATION_OPTIONS = (1 << 4),
    INITIALIZE_PARAMS_FIELD_TRACE                  = (1 << 5),

    INITIALIZE_PARAMS_FIELD_ALL = (0x3f),
    INITIALIZE_PARAMS_FIELD_REQUIRED = (INITIALIZE_PARAMS_FIELD_PROCESS_ID | INITIALIZE_PARAMS_FIELD_CAPABILITIES)
} initialize_params_fields_t;

typedef enum
{
    REFERENCE_PARAMS_FIELD_CONTEXT       = (1 << 0),
    REFERENCE_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 1),
    REFERENCE_PARAMS_FIELD_POSITION      = (1 << 2),

    REFERENCE_PARAMS_FIELD_ALL = (0x7),
    REFERENCE_PARAMS_FIELD_REQUIRED = (REFERENCE_PARAMS_FIELD_ALL)
} reference_params_fields_t;

typedef enum
{
    DOCUMENT_SYMBOL_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),

    DOCUMENT_SYMBOL_PARAMS_FIELD_ALL = (0x1),
    DOCUMENT_SYMBOL_PARAMS_FIELD_REQUIRED = (DOCUMENT_SYMBOL_PARAMS_FIELD_ALL)
} document_symbol_params_fields_t;

typedef enum
{
    WORKSPACE_SYMBOL_PARAMS_FIELD_QUERY = (1 << 0),

    WORKSPACE_SYMBOL_PARAMS_FIELD_ALL = (0x1),
    WORKSPACE_SYMBOL_PARAMS_FIELD_REQUIRED = (WORKSPACE_SYMBOL_PARAMS_FIELD_ALL)
} workspace_symbol_params_fields_t;

typedef enum
{
    DOCUMENT_LINK_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),

    DOCUMENT_LINK_PARAMS_FIELD_ALL = (0x1),
    DOCUMENT_LINK_PARAMS_FIELD_REQUIRED = (DOCUMENT_LINK_PARAMS_FIELD_ALL)
} document_link_params_fields_t;

typedef enum
{
    CODE_ACTION_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),
    CODE_ACTION_PARAMS_FIELD_RANGE         = (1 << 1),
    CODE_ACTION_PARAMS_FIELD_CONTEXT       = (1 << 2),

    CODE_ACTION_PARAMS_FIELD_ALL = (0x7),
    CODE_ACTION_PARAMS_FIELD_REQUIRED = (CODE_ACTION_PARAMS_FIELD_ALL)
} code_action_params_fields_t;



/* Client command parameter fields */
typedef enum
{
    SHOW_MESSAGE_REQUEST_PARAMS_FIELD_TYPE    = (1 << 0),
    SHOW_MESSAGE_REQUEST_PARAMS_FIELD_MESSAGE = (1 << 1),
    SHOW_MESSAGE_REQUEST_PARAMS_FIELD_ACTIONS = (1 << 2),

    SHOW_MESSAGE_REQUEST_PARAMS_FIELD_ALL = (0x7),
    SHOW_MESSAGE_REQUEST_PARAMS_FIELD_REQUIRED = (SHOW_MESSAGE_REQUEST_PARAMS_FIELD_TYPE | SHOW_MESSAGE_REQUEST_PARAMS_FIELD_MESSAGE)
} show_message_request_params_fields_t;

typedef enum
{
    SIGNATURE_HELP_FIELD_SIGNATURES       = (1 << 0),
    SIGNATURE_HELP_FIELD_ACTIVE_SIGNATURE = (1 << 1),
    SIGNATURE_HELP_FIELD_ACTIVE_PARAMETER = (1 << 2),

    SIGNATURE_HELP_FIELD_ALL = (0x7),
    SIGNATURE_HELP_FIELD_REQUIRED = (SIGNATURE_HELP_FIELD_SIGNATURES)
} signature_help_fields_t;



/* Command response parameter fields */
typedef enum
{
    INITIALIZE_RESULT_FIELD_CAPABILITIES = (1 << 0),

    INITIALIZE_RESULT_FIELD_ALL = (0x1),
    INITIALIZE_RESULT_FIELD_REQUIRED = (INITIALIZE_RESULT_FIELD_ALL)
} initialize_result_fields_t;

typedef enum
{
    SYMBOL_INFORMATION_FIELD_NAME           = (1 << 0),
    SYMBOL_INFORMATION_FIELD_KIND           = (1 << 1),
    SYMBOL_INFORMATION_FIELD_LOCATION       = (1 << 2),
    SYMBOL_INFORMATION_FIELD_CONTAINER_NAME = (1 << 3),

    SYMBOL_INFORMATION_FIELD_ALL = (0xf),
    SYMBOL_INFORMATION_FIELD_REQUIRED = (SYMBOL_INFORMATION_FIELD_NAME | SYMBOL_INFORMATION_FIELD_KIND | SYMBOL_INFORMATION_FIELD_LOCATION)
} symbol_information_fields_t;

typedef enum
{
    DOCUMENT_LINK_FIELD_RANGE  = (1 << 0),
    DOCUMENT_LINK_FIELD_TARGET = (1 << 1),

    DOCUMENT_LINK_FIELD_ALL = (0x3),
    DOCUMENT_LINK_FIELD_REQUIRED = (DOCUMENT_LINK_FIELD_RANGE)
} document_link_fields_t;

typedef enum
{
    HOVER_FIELD_CONTENTS = (1 << 0),
    HOVER_FIELD_RANGE    = (1 << 1),

    HOVER_FIELD_ALL = (0x3),
    HOVER_FIELD_REQUIRED = (HOVER_FIELD_CONTENTS)
} hover_fields_t;



/* Client notification parameter fields */
typedef enum
{
    SHOW_MESSAGE_PARAMS_FIELD_TYPE    = (1 << 0),
    SHOW_MESSAGE_PARAMS_FIELD_MESSAGE = (1 << 1),

    SHOW_MESSAGE_PARAMS_FIELD_ALL = (0x3),
    SHOW_MESSAGE_PARAMS_FIELD_REQUIRED = (SHOW_MESSAGE_PARAMS_FIELD_ALL)
} show_message_params_fields_t;

typedef enum
{
    LOG_MESSAGE_PARAMS_FIELD_TYPE    = (1 << 0),
    LOG_MESSAGE_PARAMS_FIELD_MESSAGE = (1 << 1),

    LOG_MESSAGE_PARAMS_FIELD_ALL = (0x3),
    LOG_MESSAGE_PARAMS_FIELD_REQUIRED = (LOG_MESSAGE_PARAMS_FIELD_ALL)
} log_message_params_fields_t;

typedef enum
{
    PUBLISH_DIAGNOSTICS_PARAMS_FIELD_URI         = (1 << 0),
    PUBLISH_DIAGNOSTICS_PARAMS_FIELD_DIAGNOSTICS = (1 << 1),

    PUBLISH_DIAGNOSTICS_PARAMS_FIELD_ALL = (0x3),
    PUBLISH_DIAGNOSTICS_PARAMS_FIELD_REQUIRED = (PUBLISH_DIAGNOSTICS_PARAMS_FIELD_ALL)
} publish_diagnostics_params_fields_t;



/* Server notification parameter fields */
typedef enum
{
    DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),

    DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_ALL = (0x1),
    DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED = (DID_OPEN_TEXT_DOCUMENT_PARAMS_FIELD_ALL)
} did_open_text_document_params_fields_t;

typedef enum
{
    DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT   = (1 << 0),
    DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_CONTENT_CHANGES = (1 << 1),

    DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_ALL = (0x3),
    DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED = (DID_CHANGE_TEXT_DOCUMENT_PARAMS_FIELD_ALL)
} did_change_text_document_params_fields_t;

typedef enum
{
    DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),
    DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT          = (1 << 1),

    DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_ALL = (0x3),
    DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED = (DID_SAVE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT)
} did_save_text_document_params_fields_t;

typedef enum
{
    DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_TEXT_DOCUMENT = (1 << 0),

    DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_ALL = (0x1),
    DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_REQUIRED = (DID_CLOSE_TEXT_DOCUMENT_PARAMS_FIELD_ALL)
} did_close_text_document_params_fields_t;

typedef enum
{
    DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_CHANGES = (1 << 0),

    DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_ALL = (0x1),
    DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_REQUIRED = (DID_CHANGE_WATCHED_FILES_PARAMS_FIELD_ALL)
} did_change_watched_files_params_fields_t;


/*******************************************************************************
 * Parameter structures
 ******************************************************************************/
/* Common parameter structures */
typedef struct
{
    compilation_database_params_fields_t valid_fields;

    char * path;
    char * * p_additional_arguments;
    uint32_t additional_arguments_count;
} compilation_database_params_t;

typedef struct
{
    initialization_options_fields_t valid_fields;

    char * * p_flags;
    uint32_t flags_count;
    compilation_database_params_t * p_compilation_database;
    uint32_t compilation_database_count;
} initialization_options_t;

typedef struct
{
    workspace_client_capabilities_fields_t valid_fields;

    bool apply_edit;
    struct
    {
        bool dynamic_registration;
    } workspace_edit;
    struct
    {
        bool dynamic_registration;
    } did_change_configuration;
    struct
    {
        bool dynamic_registration;
    } did_change_watched_files;
    struct
    {
        bool dynamic_registration;
    } symbol;
    struct
    {
        bool dynamic_registration;
    } execute_command;
} workspace_client_capabilities_t;

typedef struct
{
    text_document_client_capabilities_fields_t valid_fields;

    struct
    {
        bool dynamic_registration;
        bool will_save;
        bool will_save_wait_until;
        bool did_save;
    } synchronization;
    struct
    {
        bool snippet_support;
        bool commit_characters_support;
    } completion;
    struct
    {
        bool dynamic_registration;
    } hover;
    struct
    {
        bool dynamic_registration;
        struct
        {
            char * * p_documentation_format;
            uint32_t documentation_format_count;
        } signature_information;
    } signature_help;
    struct
    {
        bool dynamic_registration;
    } references;
    struct
    {
        bool dynamic_registration;
    } document_highlight;
    struct
    {
        bool dynamic_registration;
    } document_symbol;
    struct
    {
        bool dynamic_registration;
    } formatting;
    struct
    {
        bool dynamic_registration;
    } on_type_formatting;
    struct
    {
        bool dynamic_registration;
    } definition;
    struct
    {
        bool dynamic_registration;
    } code_action;
    struct
    {
        bool dynamic_registration;
    } code_lens;
    struct
    {
        bool dynamic_registration;
    } document_link;
    struct
    {
        bool dynamic_registration;
    } rename;
} text_document_client_capabilities_t;

typedef struct
{
    client_capabilities_fields_t valid_fields;

    workspace_client_capabilities_t workspace;
    text_document_client_capabilities_t text_document;
    json_t * experimental;
} client_capabilities_t;

typedef struct
{
    completion_options_fields_t valid_fields;

    bool resolve_provider;
    char * * p_trigger_characters;
    uint32_t trigger_characters_count;
} completion_options_t;

typedef struct
{
    signature_help_options_fields_t valid_fields;

    char * * p_trigger_characters;
    uint32_t trigger_characters_count;
} signature_help_options_t;

typedef struct
{
    code_lens_options_fields_t valid_fields;

    bool resolve_provider;
} code_lens_options_t;

typedef struct
{
    save_options_fields_t valid_fields;

    bool include_text;
} save_options_t;

typedef struct
{
    text_document_sync_options_fields_t valid_fields;

    bool open_close;
    text_document_sync_kind_t change;
    bool will_save;
    bool will_save_wait_until;
    save_options_t save;
} text_document_sync_options_t;

typedef struct
{
    server_capabilities_fields_t valid_fields;

    text_document_sync_options_t text_document_sync;
    bool hover_provider;
    completion_options_t completion_provider;
    signature_help_options_t signature_help_provider;
    bool definition_provider;
    bool references_provider;
    bool document_highlight_provider;
    bool document_symbol_provider;
    bool workspace_symbol_provider;
    bool code_action_provider;
    code_lens_options_t code_lens_provider;
    bool document_formatting_provider;
} server_capabilities_t;

typedef struct
{
    markup_content_fields_t valid_fields;

    char * kind;
    char * value;
} markup_content_t;

typedef struct
{
    marked_string_fields_t valid_fields;

    char * language;
    char * value;
} marked_string_t;

typedef struct
{
    message_action_item_fields_t valid_fields;

    char * title;
} message_action_item_t;

typedef struct
{
    position_fields_t valid_fields;

    int64_t line;
    int64_t character;
} position_t;

typedef struct
{
    range_fields_t valid_fields;

    position_t start;
    position_t end;
} range_t;

typedef struct
{
    text_edit_fields_t valid_fields;

    range_t range;
    char * new_text;
} text_edit_t;

typedef struct
{
    location_fields_t valid_fields;

    uri_t uri;
    range_t range;
} location_t;

typedef struct
{
    diagnostic_related_information_fields_t valid_fields;

    location_t location;
    char * message;
} diagnostic_related_information_t;

typedef struct
{
    diagnostic_fields_t valid_fields;

    range_t range;
    diagnostic_severity_t severity;
    int64_t code;
    char * source;
    char * message;
    diagnostic_related_information_t * p_related_information;
    uint32_t related_information_count;
} diagnostic_t;

typedef struct
{
    text_document_item_fields_t valid_fields;

    uri_t uri;
    char * language_id;
    int64_t version;
    char * text;
} text_document_item_t;

typedef struct
{
    text_document_content_change_event_fields_t valid_fields;

    range_t range;
    int64_t range_length;
    char * text;
} text_document_content_change_event_t;

typedef struct
{
    text_document_identifier_fields_t valid_fields;

    uri_t uri;
} text_document_identifier_t;

typedef struct
{
    versioned_text_document_identifier_fields_t valid_fields;

    uri_t uri;
    int64_t version;
} versioned_text_document_identifier_t;

typedef struct
{
    file_event_fields_t valid_fields;

    uri_t uri;
    file_change_type_t type;
} file_event_t;

typedef struct
{
    text_document_position_params_fields_t valid_fields;

    text_document_identifier_t text_document;
    position_t position;
} text_document_position_params_t;

typedef struct
{
    command_fields_t valid_fields;

    char * title;
    char * command;
    json_t * arguments;
} command_t;

typedef struct
{
    completion_item_fields_t valid_fields;

    char * label;
    completion_item_kind_t kind;
    char * detail;
    markup_content_t documentation;
    char * sort_text;
    char * filter_text;
    char * insert_text;
    insert_text_format_t insert_text_format;
    text_edit_t text_edit;
    text_edit_t * p_additional_text_edits;
    uint32_t additional_text_edits_count;
    char * * p_commit_characters;
    uint32_t commit_characters_count;
    command_t command;
    json_t * data;
} completion_item_t;

typedef struct
{
    parameter_information_fields_t valid_fields;

    char * label;
    markup_content_t documentation;
} parameter_information_t;

typedef struct
{
    signature_information_fields_t valid_fields;

    char * label;
    markup_content_t documentation;
    parameter_information_t * p_parameters;
    uint32_t parameters_count;
} signature_information_t;

typedef struct
{
    reference_context_fields_t valid_fields;

    bool include_declaration;
} reference_context_t;

typedef struct
{
    completion_list_fields_t valid_fields;

    bool is_incomplete;
    completion_item_t * p_items;
    uint32_t items_count;
} completion_list_t;

typedef struct
{
    code_action_context_fields_t valid_fields;

    diagnostic_t * p_diagnostics;
    uint32_t diagnostics_count;
} code_action_context_t;

/* Server command parameter structures */
typedef struct
{
    initialize_params_fields_t valid_fields;

    int64_t process_id;
    char * root_path;
    uri_t root_uri;
    client_capabilities_t capabilities;
    initialization_options_t initialization_options;
    char * trace;
} initialize_params_t;

typedef struct
{
    reference_params_fields_t valid_fields;

    reference_context_t context;
    text_document_identifier_t text_document;
    position_t position;
} reference_params_t;

typedef struct
{
    document_symbol_params_fields_t valid_fields;

    text_document_identifier_t text_document;
} document_symbol_params_t;

typedef struct
{
    workspace_symbol_params_fields_t valid_fields;

    char * query;
} workspace_symbol_params_t;

typedef struct
{
    document_link_params_fields_t valid_fields;

    text_document_identifier_t text_document;
} document_link_params_t;

typedef struct
{
    code_action_params_fields_t valid_fields;

    text_document_identifier_t text_document;
    range_t range;
    code_action_context_t context;
} code_action_params_t;

/* Client command parameter structures */
typedef struct
{
    show_message_request_params_fields_t valid_fields;

    message_type_t type;
    char * message;
    message_action_item_t * p_actions;
    uint32_t actions_count;
} show_message_request_params_t;

typedef struct
{
    signature_help_fields_t valid_fields;

    signature_information_t * p_signatures;
    uint32_t signatures_count;
    int64_t active_signature;
    int64_t active_parameter;
} signature_help_t;

/* Command response parameter structures */
typedef struct
{
    initialize_result_fields_t valid_fields;

    server_capabilities_t capabilities;
} initialize_result_t;

typedef struct
{
    symbol_information_fields_t valid_fields;

    char * name;
    symbol_kind_t kind;
    location_t location;
    char * container_name;
} symbol_information_t;

typedef struct
{
    document_link_fields_t valid_fields;

    range_t range;
    uri_t target;
} document_link_t;

typedef struct
{
    hover_fields_t valid_fields;

    markup_content_t contents;
    range_t range;
} hover_t;

/* Client notification parameter structures */
typedef struct
{
    show_message_params_fields_t valid_fields;

    message_type_t type;
    char * message;
} show_message_params_t;

typedef struct
{
    log_message_params_fields_t valid_fields;

    message_type_t type;
    char * message;
} log_message_params_t;

typedef struct
{
    publish_diagnostics_params_fields_t valid_fields;

    uri_t uri;
    diagnostic_t * p_diagnostics;
    uint32_t diagnostics_count;
} publish_diagnostics_params_t;

/* Server notification parameter structures */
typedef struct
{
    did_open_text_document_params_fields_t valid_fields;

    text_document_item_t text_document;
} did_open_text_document_params_t;

typedef struct
{
    did_change_text_document_params_fields_t valid_fields;

    versioned_text_document_identifier_t text_document;
    text_document_content_change_event_t * p_content_changes;
    uint32_t content_changes_count;
} did_change_text_document_params_t;

typedef struct
{
    did_save_text_document_params_fields_t valid_fields;

    text_document_identifier_t text_document;
    char * text;
} did_save_text_document_params_t;

typedef struct
{
    did_close_text_document_params_fields_t valid_fields;

    text_document_identifier_t text_document;
} did_close_text_document_params_t;

typedef struct
{
    did_change_watched_files_params_fields_t valid_fields;

    file_event_t * p_changes;
    uint32_t changes_count;
} did_change_watched_files_params_t;