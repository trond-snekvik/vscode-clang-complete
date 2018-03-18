#include <stdlib.h>
#include <stdbool.h>
#include "json_rpc.h"
#include "log.h"

#define CONTENT_LENGTH_HEADER   "Content-Length: %u\r\n\r\n"

typedef struct
{
    const char * p_method;
    json_rpc_request_handler_t callback;
} request_handler_t;

typedef struct
{
    const char * p_method;
    json_rpc_notification_handler_t callback;
} notification_handler_t;

typedef struct response_waiter
{
    json_rpc_request_id_t id;
    json_rpc_response_handler_t handler;
    struct response_waiter * p_next;
} response_waiter_t;

static struct
{
    unsigned count;
    request_handler_t * p_handlers;
} m_request_handlers;

static struct
{
    unsigned count;
    notification_handler_t * p_handlers;
} m_notification_handlers;

static struct
{
    response_waiter_t * p_head;
    response_waiter_t * p_tail;
} m_response_waiters;

static bool m_running;
static json_rpc_request_id_t m_request_id;
static unsigned m_responses_sent;

static void send(json_t * p_message)
{
    const char * p_value = json_dumps(p_message, 0);
    char header_buf[128];
    sprintf(header_buf, "Content-Length: %u\n\n", strlen(p_value));
    OUTPUT(header_buf);
    OUTPUT(p_value);
}

static const request_handler_t * find_request_handler(const char * p_method)
{
    for (unsigned i = 0; i < m_request_handlers.count; ++i)
    {
        if (strcmp(m_request_handlers.p_handlers[i].p_method, p_method) == 0)
        {
            return &m_request_handlers.p_handlers[i];
        }
    }
    return NULL;
}

static const notification_handler_t * find_notification_handler(const char * p_method)
{
    for (unsigned i = 0; i < m_notification_handlers.count; ++i)
    {
        if (strcmp(m_notification_handlers.p_handlers[i].p_method, p_method) == 0)
        {
            return &m_notification_handlers.p_handlers[i];
        }
    }
    return NULL;
}

static void handle_incoming(json_t * p_root)
{
    if (p_root)
    {
        //LOG("%s\n", json_dumps(p_root, 0));
        if (json_is_array(p_root))
        {
            unsigned index;
            json_t * p_it;
            json_array_foreach(p_root, index, p_it)
            {
                handle_incoming(p_it);
            }
        }
        else if (json_is_object(p_root))
        {
            json_t * p_jsonrpc = json_object_get(p_root, "jsonrpc");
            json_t * p_method = json_object_get(p_root, "method");
            json_t * p_params = json_object_get(p_root, "params");
            json_t * p_id = json_object_get(p_root, "id");
            json_t * p_result = json_object_get(p_root, "result");
            json_t * p_error = json_object_get(p_root, "error");

            // determine message type
            if (p_jsonrpc)
            {
                if (p_method)
                {
                    // request or notification
                    bool handled = false;
                    json_t * p_response = NULL;
                    if (p_id)
                    {
                        LOG("GOT REQUEST \"%s\"\n", json_string_value(p_method));
                        p_response = json_object();
                        json_object_set_new(p_response, "jsonrpc", json_string("2.0"));
                        json_object_set(p_response, "id", p_id);

                        const request_handler_t * p_handler = find_request_handler(json_string_value(p_method));
                        if (p_handler)
                        {
                            unsigned responses_before = m_responses_sent;
                            p_handler->callback(p_handler->p_method, p_params, p_response);
                            if (p_id)
                            {
                                // Must send exactly one response to a message with an ID
                                ASSERT(m_responses_sent == responses_before + 1);
                            }
                        }
                        else
                        {
                            json_rpc_error_response_send(p_response, JSON_RPC_ERROR_METHOD_NOT_FOUND, "No handler registered for method", NULL);
                        }
                    }
                    else
                    {
                        LOG("GOT NOTIFICATION \"%s\"\n", json_string_value(p_method));

                        const notification_handler_t * p_handler = find_notification_handler(json_string_value(p_method));
                        if (p_handler)
                        {
                            unsigned responses_before = m_responses_sent;
                            p_handler->callback(p_handler->p_method, p_params);
                        }
                        else
                        {
                            LOG("No handler.\n");
                        }
                    }
                    json_decref(p_response);
                }
                else if (p_id && json_is_integer(p_id))
                {
                    // response
                    json_rpc_request_id_t id = json_integer_value(p_id);
                    if (p_result || p_error)
                    {
                        while (m_response_waiters.p_head != NULL)
                        {
                            // expect in-order responses
                            if (m_response_waiters.p_head->id < id)
                            {
                                json_rpc_response_params_t params;
                                params.error.code = JSON_RPC_ERROR_NO_RESPONSE;
                                params.error.p_data = NULL;
                                params.error.p_message = "No response from client";
                                m_response_waiters.p_head->handler(id, JSON_RPC_RESULT_ERROR, &params);
                            }
                            else if (m_response_waiters.p_head->id == id)
                            {
                                if (p_result)
                                {
                                    json_rpc_response_params_t params;
                                    params.success.p_params = p_params;
                                    m_response_waiters.p_head->handler(id, JSON_RPC_RESULT_SUCCESS, &params);
                                }
                                else if (p_error)
                                {
                                    json_t * p_code     = json_object_get(p_error, "code");
                                    json_t * p_message  = json_object_get(p_error, "message");
                                    json_t * p_data     = json_object_get(p_error, "data");

                                    if (p_code && json_is_integer(p_code) && (!p_message || json_is_string(p_message)))
                                    {
                                        json_rpc_response_params_t params;
                                        params.error.code = JSON_RPC_ERROR_NO_RESPONSE;
                                        params.error.p_data = p_data;
                                        if (p_message)
                                        {
                                            params.error.p_message = json_string_value(p_message);
                                        }
                                        else
                                        {
                                            params.error.p_message = "";
                                        }
                                        m_response_waiters.p_head->handler(id, JSON_RPC_RESULT_ERROR, &params);
                                    }
                                    json_decref(p_code);
                                    json_decref(p_message);
                                    json_decref(p_data);
                                }
                            }
                            else
                            {
                                break;
                            }

                            if (m_response_waiters.p_head == m_response_waiters.p_tail)
                            {
                                m_response_waiters.p_tail = NULL;
                            }

                            response_waiter_t * p_new_head = m_response_waiters.p_head->p_next;
                            FREE(m_response_waiters.p_head);
                            m_response_waiters.p_head = p_new_head;
                        }
                    }
                }
            }

            json_decref(p_jsonrpc);
            json_decref(p_method);
            json_decref(p_params);
            json_decref(p_id);
            json_decref(p_result);
            json_decref(p_error);
        }
        json_decref(p_root);
    }
    else
    {
        LOG("Parsing failed\n");
    }
}

void json_rpc_init(void)
{
}

void json_rpc_request_handler_add(const char * p_method, json_rpc_request_handler_t request_handler)
{
    ASSERT(p_method);
    // no duplicate handlers
    ASSERT(find_request_handler(p_method) == NULL);
    LOG("Adding handler for %s\n", p_method);
    m_request_handlers.count++;
    m_request_handlers.p_handlers = REALLOC(m_request_handlers.p_handlers, m_request_handlers.count * sizeof(request_handler_t));
    m_request_handlers.p_handlers[m_request_handlers.count - 1].p_method = p_method;
    m_request_handlers.p_handlers[m_request_handlers.count - 1].callback = request_handler;
}

void json_rpc_notification_handler_add(const char * p_method, json_rpc_notification_handler_t notification_handler)
{
    ASSERT(p_method);
    // no duplicate handlers
    ASSERT(find_request_handler(p_method) == NULL);
    LOG("Adding handler for %s\n", p_method);
    m_notification_handlers.count++;
    m_notification_handlers.p_handlers = REALLOC(m_notification_handlers.p_handlers, m_notification_handlers.count * sizeof(notification_handler_t));
    m_notification_handlers.p_handlers[m_notification_handlers.count - 1].p_method = p_method;
    m_notification_handlers.p_handlers[m_notification_handlers.count - 1].callback = notification_handler;
}

void json_rpc_response_send(json_t * p_response, json_t * p_result)
{
    ASSERT(p_response);
    ASSERT(p_result);
    json_object_set_new(p_response, "result", p_result);
    LOG("Sent response: %s\n", json_dumps(p_response, 0));
    send(p_response);
    m_responses_sent++;
}

void json_rpc_error_response_send(json_t * p_response, int code, const char * p_message, json_t * p_data)
{
    ASSERT(p_response);
    ASSERT(p_message);

    LOG("Sending error response: E %d: %s\n", code, p_message);

    json_t * p_error = json_object();
    json_object_set_new(p_error, "code", json_integer(code));
    json_object_set_new(p_error, "message", json_string(p_message));
    if (p_data)
    {
        json_object_set_new(p_error, "data", p_data);
    }
    json_object_set_new(p_response, "error", p_error);
    send(p_response);
    m_responses_sent++;
}

json_rpc_request_id_t json_rpc_request_send(const char * p_method, json_t * p_params, json_rpc_response_handler_t response_handler)
{
    if (response_handler)
    {
        response_waiter_t * p_waiter = MALLOC(sizeof(response_waiter_t));
        ASSERT(p_waiter);

        p_waiter->id = m_request_id;
        p_waiter->handler = response_handler;
        p_waiter->p_next = NULL;
        if (m_response_waiters.p_tail)
        {
            m_response_waiters.p_tail->p_next = p_waiter;
        }
        else
        {
            m_response_waiters.p_head = p_waiter;
        }
        m_response_waiters.p_tail = p_waiter;
    }

    json_t * p_request = json_object();
    ASSERT(p_request);
    json_object_set_new(p_request, "jsonrpc", json_string("2.0"));
    json_object_set_new(p_request, "method", json_string(p_method));
    json_object_set_new(p_request, "id", json_integer(m_request_id));
    if (p_params)
    {
        json_object_set_new(p_request, "params", p_params);
    }

    send(p_request);
    json_decref(p_request);
    return m_request_id++;
}

void json_rpc_notification_send(const char * p_method, json_t * p_params)
{
    json_t * p_notification = json_object();
    ASSERT(p_notification);
    json_object_set_new(p_notification, "jsonrpc", json_string("2.0"));
    json_object_set_new(p_notification, "method", json_string(p_method));
    if (p_params)
    {
        json_object_set_new(p_notification, "params", p_params);
    }

    send(p_notification);
    json_decref(p_notification);
}


void json_rpc_listen(FILE * stream)
{
    ASSERT(stream);
    LOG("Listening for incoming rpc\n");
    m_running = true;
    size_t buffer_size = 128;
    char * p_buffer = MALLOC(buffer_size);
    while (m_running)
    {
        size_t length;
        if (scanf(CONTENT_LENGTH_HEADER, &length))
        {
            if (buffer_size < length)
            {
                buffer_size = length + 1;
                p_buffer = REALLOC(p_buffer, buffer_size);
                ASSERT(p_buffer);
            }

            char * p_c = p_buffer;
            do
            {
                size_t count = fread(p_c, 1, min((size_t)(&p_buffer[buffer_size] - p_c), length), stream);
                p_c += count;
            } while ((size_t)(p_c - p_buffer) < length);
            *p_c = '\0';

            LOG("Handling buffer: %s\n", p_buffer);
            json_error_t err;
            json_t * p_json = json_loads(p_buffer, JSON_DISABLE_EOF_CHECK, &err);
            if (p_json)
            {
                handle_incoming(p_json);
            }
            else
            {
                LOG("Parsing failed: L%u:%u: %s\n", err.line, err.column, err.text);
            }
        }

    }
    LOG("Exiting\n");
    FREE(p_buffer);
}

void json_rpc_stop(void)
{
    m_running = false;
}