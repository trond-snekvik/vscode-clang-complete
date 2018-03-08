#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
#include "structures.h"

#include "encoders.h"
#include "decoders.h"
#include "json_rpc.h"
#include "log.h"
#include "message_handling.h"

/*******************************************************************************
 * Message names
 ******************************************************************************/
#define LSP_REQUEST_INITIALIZE "initialize"
#define LSP_REQUEST_WORKSPACE_SYMBOL "workspace/symbol"
#define LSP_REQUEST_TEXT_DOCUMENT_COMPLETION "textDocument/completion"
#define LSP_REQUEST_TEXT_DOCUMENT_SIGNATURE_HELP "textDocument/signatureHelp"
#define LSP_REQUEST_TEXT_DOCUMENT_DEFINITION "textDocument/definition"
#define LSP_REQUEST_TEXT_DOCUMENT_REFERENCES "textDocument/references"
#define LSP_REQUEST_TEXT_DOCUMENT_DOCUMENT_LINK "textDocument/documentLink"
#define LSP_REQUEST_TEXT_DOCUMENT_HOVER "textDocument/hover"
#define LSP_REQUEST_TEXT_DOCUMENT_CODE_ACTION "textDocument/codeAction"
#define LSP_NOTIFICATION_TEXT_DOCUMENT_DID_OPEN "textDocument/didOpen"
#define LSP_NOTIFICATION_TEXT_DOCUMENT_DID_CHANGE "textDocument/didChange"
#define LSP_NOTIFICATION_TEXT_DOCUMENT_DID_SAVE "textDocument/didSave"
#define LSP_NOTIFICATION_TEXT_DOCUMENT_DID_CLOSE "textDocument/didClose"

/*******************************************************************************
 * Handler function pointers
 ******************************************************************************/
static lsp_request_handler_initialize_t mp_request_handler_initialize;
static lsp_request_handler_workspace_symbol_t mp_request_handler_workspace_symbol;
static lsp_request_handler_text_document_completion_t mp_request_handler_text_document_completion;
static lsp_request_handler_text_document_signature_help_t mp_request_handler_text_document_signature_help;
static lsp_request_handler_text_document_definition_t mp_request_handler_text_document_definition;
static lsp_request_handler_text_document_references_t mp_request_handler_text_document_references;
static lsp_request_handler_text_document_document_link_t mp_request_handler_text_document_document_link;
static lsp_request_handler_text_document_hover_t mp_request_handler_text_document_hover;
static lsp_request_handler_text_document_code_action_t mp_request_handler_text_document_code_action;
static lsp_notification_handler_text_document_did_open_t mp_notification_handler_text_document_did_open;
static lsp_notification_handler_text_document_did_change_t mp_notification_handler_text_document_did_change;
static lsp_notification_handler_text_document_did_save_t mp_notification_handler_text_document_did_save;
static lsp_notification_handler_text_document_did_close_t mp_notification_handler_text_document_did_close;

/*******************************************************************************
 * Handler functions
 ******************************************************************************/
static void request_handler_initialize(const char * p_method, json_t * p_params, json_t * p_response)
{
    initialize_params_t params = decode_initialize_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_initialize(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_initialize_params(params);
}
static void request_handler_workspace_symbol(const char * p_method, json_t * p_params, json_t * p_response)
{
    workspace_symbol_params_t params = decode_workspace_symbol_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_workspace_symbol(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_workspace_symbol_params(params);
}
static void request_handler_text_document_completion(const char * p_method, json_t * p_params, json_t * p_response)
{
    text_document_position_params_t params = decode_text_document_position_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_completion(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_text_document_position_params(params);
}
static void request_handler_text_document_signature_help(const char * p_method, json_t * p_params, json_t * p_response)
{
    text_document_position_params_t params = decode_text_document_position_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_signature_help(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_text_document_position_params(params);
}
static void request_handler_text_document_definition(const char * p_method, json_t * p_params, json_t * p_response)
{
    text_document_position_params_t params = decode_text_document_position_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_definition(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_text_document_position_params(params);
}
static void request_handler_text_document_references(const char * p_method, json_t * p_params, json_t * p_response)
{
    reference_params_t params = decode_reference_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_references(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_reference_params(params);
}
static void request_handler_text_document_document_link(const char * p_method, json_t * p_params, json_t * p_response)
{
    document_link_params_t params = decode_document_link_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_document_link(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_document_link_params(params);
}
static void request_handler_text_document_hover(const char * p_method, json_t * p_params, json_t * p_response)
{
    text_document_position_params_t params = decode_text_document_position_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_hover(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_text_document_position_params(params);
}
static void request_handler_text_document_code_action(const char * p_method, json_t * p_params, json_t * p_response)
{
    code_action_params_t params = decode_code_action_params(p_params);
    if (decoder_error() == DECODER_ERROR_NONE)
    {
        mp_request_handler_text_document_code_action(&params, p_response);
    }
    else
    {
        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);
    }
    free_code_action_params(params);
}
static void notification_handler_text_document_did_open(const char * p_method, json_t * p_params)
{
    did_open_text_document_params_t params = decode_did_open_text_document_params(p_params);
    mp_notification_handler_text_document_did_open(&params);
    free_did_open_text_document_params(params);
}
static void notification_handler_text_document_did_change(const char * p_method, json_t * p_params)
{
    did_change_text_document_params_t params = decode_did_change_text_document_params(p_params);
    mp_notification_handler_text_document_did_change(&params);
    free_did_change_text_document_params(params);
}
static void notification_handler_text_document_did_save(const char * p_method, json_t * p_params)
{
    did_save_text_document_params_t params = decode_did_save_text_document_params(p_params);
    mp_notification_handler_text_document_did_save(&params);
    free_did_save_text_document_params(params);
}
static void notification_handler_text_document_did_close(const char * p_method, json_t * p_params)
{
    did_close_text_document_params_t params = decode_did_close_text_document_params(p_params);
    mp_notification_handler_text_document_did_close(&params);
    free_did_close_text_document_params(params);
}

/*******************************************************************************
 * Register functions
 ******************************************************************************/
void lsp_request_handler_initialize_register(lsp_request_handler_initialize_t handler)
{
    mp_request_handler_initialize = handler;
    json_rpc_request_handler_add(LSP_REQUEST_INITIALIZE, request_handler_initialize);
}
void lsp_request_handler_workspace_symbol_register(lsp_request_handler_workspace_symbol_t handler)
{
    mp_request_handler_workspace_symbol = handler;
    json_rpc_request_handler_add(LSP_REQUEST_WORKSPACE_SYMBOL, request_handler_workspace_symbol);
}
void lsp_request_handler_text_document_completion_register(lsp_request_handler_text_document_completion_t handler)
{
    mp_request_handler_text_document_completion = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_COMPLETION, request_handler_text_document_completion);
}
void lsp_request_handler_text_document_signature_help_register(lsp_request_handler_text_document_signature_help_t handler)
{
    mp_request_handler_text_document_signature_help = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_SIGNATURE_HELP, request_handler_text_document_signature_help);
}
void lsp_request_handler_text_document_definition_register(lsp_request_handler_text_document_definition_t handler)
{
    mp_request_handler_text_document_definition = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_DEFINITION, request_handler_text_document_definition);
}
void lsp_request_handler_text_document_references_register(lsp_request_handler_text_document_references_t handler)
{
    mp_request_handler_text_document_references = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_REFERENCES, request_handler_text_document_references);
}
void lsp_request_handler_text_document_document_link_register(lsp_request_handler_text_document_document_link_t handler)
{
    mp_request_handler_text_document_document_link = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_DOCUMENT_LINK, request_handler_text_document_document_link);
}
void lsp_request_handler_text_document_hover_register(lsp_request_handler_text_document_hover_t handler)
{
    mp_request_handler_text_document_hover = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_HOVER, request_handler_text_document_hover);
}
void lsp_request_handler_text_document_code_action_register(lsp_request_handler_text_document_code_action_t handler)
{
    mp_request_handler_text_document_code_action = handler;
    json_rpc_request_handler_add(LSP_REQUEST_TEXT_DOCUMENT_CODE_ACTION, request_handler_text_document_code_action);
}
void lsp_notification_handler_text_document_did_open_register(lsp_notification_handler_text_document_did_open_t handler)
{
    mp_notification_handler_text_document_did_open = handler;
    json_rpc_notification_handler_add(LSP_NOTIFICATION_TEXT_DOCUMENT_DID_OPEN, notification_handler_text_document_did_open);
}
void lsp_notification_handler_text_document_did_change_register(lsp_notification_handler_text_document_did_change_t handler)
{
    mp_notification_handler_text_document_did_change = handler;
    json_rpc_notification_handler_add(LSP_NOTIFICATION_TEXT_DOCUMENT_DID_CHANGE, notification_handler_text_document_did_change);
}
void lsp_notification_handler_text_document_did_save_register(lsp_notification_handler_text_document_did_save_t handler)
{
    mp_notification_handler_text_document_did_save = handler;
    json_rpc_notification_handler_add(LSP_NOTIFICATION_TEXT_DOCUMENT_DID_SAVE, notification_handler_text_document_did_save);
}
void lsp_notification_handler_text_document_did_close_register(lsp_notification_handler_text_document_did_close_t handler)
{
    mp_notification_handler_text_document_did_close = handler;
    json_rpc_notification_handler_add(LSP_NOTIFICATION_TEXT_DOCUMENT_DID_CLOSE, notification_handler_text_document_did_close);
}
