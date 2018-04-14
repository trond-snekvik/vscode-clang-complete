#pragma once
#include <jansson.h>
#include <stdio.h>
#include <stdint.h>

#define JSON_RPC_ERROR_PARSE_ERROR      (-32700)
#define JSON_RPC_ERROR_INVALID_REQUEST  (-32600)
#define JSON_RPC_ERROR_METHOD_NOT_FOUND (-32601)
#define JSON_RPC_ERROR_INVALID_PARAMS   (-32602)
#define JSON_RPC_ERROR_INTERNAL_ERROR   (-32603)
#define JSON_RPC_ERROR_NO_RESPONSE      (-32000)

typedef enum
{
    JSON_RPC_RESULT_SUCCESS,
    JSON_RPC_RESULT_ERROR,
} json_rpc_result_t;

typedef union
{
    struct
    {
        json_t * p_params;
    } success;
    struct
    {
        int code;
        const char * p_message; ///< May be NULL
        json_t * p_data; ///< May be NULL
    } error;
} json_rpc_response_params_t;

typedef int64_t json_rpc_request_id_t;

typedef void (*json_rpc_request_handler_t)(const char * p_method, json_t * p_params, json_t * p_response);
typedef void (*json_rpc_response_handler_t)(json_rpc_request_id_t id, json_rpc_result_t result, const json_rpc_response_params_t * p_params);
typedef void (*json_rpc_notification_handler_t)(const char * p_method, json_t * p_params);


void json_rpc_init(void);
void json_rpc_request_handler_add(const char * p_method, json_rpc_request_handler_t request_handler);
void json_rpc_notification_handler_add(const char * p_method, json_rpc_notification_handler_t notification_handler);

void json_rpc_response_send(json_t * p_response, json_t * p_result);
void json_rpc_error_response_send(json_t * p_response, int code, const char * p_message, json_t * p_data);

json_rpc_request_id_t json_rpc_request_send(const char * p_method, json_t * p_params, json_rpc_response_handler_t response_handler);
void json_rpc_notification_send(const char * p_method, json_t * p_params);

void json_rpc_listen(FILE * stream);
void json_rpc_stop(void);

void json_rpc_suspend(void);
void json_rpc_resume(void);