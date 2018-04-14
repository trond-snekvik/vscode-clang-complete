#pragma once
#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
#include "structures.h"


/*******************************************************************************
 * Outgoing messages
 ******************************************************************************/
#define LSP_NOTIFICATION_TEXT_DOCUMENT_PUBLISH_DIAGNOSTICS "textDocument/publishDiagnostics"
#define LSP_NOTIFICATION_WINDOW_SHOW_MESSAGE "window/showMessage"
#define LSP_NOTIFICATION_WINDOW_LOG_MESSAGE "window/logMessage"
#define LSP_NOTIFICATION_TELEMETRY_EVENT "telemetry/event"
#define LSP_REQUEST_WINDOW_SHOW_MESSAGE_REQUEST "window/showMessageRequest"

/*******************************************************************************
 * Request handler types
 ******************************************************************************/
typedef void (*lsp_request_handler_initialize_t)(const initialize_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_workspace_symbol_t)(const workspace_symbol_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_document_symbol_t)(const document_symbol_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_completion_t)(const text_document_position_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_signature_help_t)(const text_document_position_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_definition_t)(const text_document_position_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_references_t)(const reference_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_document_link_t)(const document_link_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_hover_t)(const text_document_position_params_t * p_params, json_t * p_response);
typedef void (*lsp_request_handler_text_document_code_action_t)(const code_action_params_t * p_params, json_t * p_response);

/*******************************************************************************
 * Notification handler types
 ******************************************************************************/
typedef void (*lsp_notification_handler_text_document_did_open_t)(const did_open_text_document_params_t * p_params);
typedef void (*lsp_notification_handler_text_document_did_change_t)(const did_change_text_document_params_t * p_params);
typedef void (*lsp_notification_handler_text_document_did_save_t)(const did_save_text_document_params_t * p_params);
typedef void (*lsp_notification_handler_text_document_did_close_t)(const did_close_text_document_params_t * p_params);

/*******************************************************************************
 * Register functions
 ******************************************************************************/
void lsp_request_handler_initialize_register(lsp_request_handler_initialize_t handler);
void lsp_request_handler_workspace_symbol_register(lsp_request_handler_workspace_symbol_t handler);
void lsp_request_handler_text_document_document_symbol_register(lsp_request_handler_text_document_document_symbol_t handler);
void lsp_request_handler_text_document_completion_register(lsp_request_handler_text_document_completion_t handler);
void lsp_request_handler_text_document_signature_help_register(lsp_request_handler_text_document_signature_help_t handler);
void lsp_request_handler_text_document_definition_register(lsp_request_handler_text_document_definition_t handler);
void lsp_request_handler_text_document_references_register(lsp_request_handler_text_document_references_t handler);
void lsp_request_handler_text_document_document_link_register(lsp_request_handler_text_document_document_link_t handler);
void lsp_request_handler_text_document_hover_register(lsp_request_handler_text_document_hover_t handler);
void lsp_request_handler_text_document_code_action_register(lsp_request_handler_text_document_code_action_t handler);
void lsp_notification_handler_text_document_did_open_register(lsp_notification_handler_text_document_did_open_t handler);
void lsp_notification_handler_text_document_did_change_register(lsp_notification_handler_text_document_did_change_t handler);
void lsp_notification_handler_text_document_did_save_register(lsp_notification_handler_text_document_did_save_t handler);
void lsp_notification_handler_text_document_did_close_register(lsp_notification_handler_text_document_did_close_t handler);
