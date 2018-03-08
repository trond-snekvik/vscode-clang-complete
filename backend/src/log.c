#include "log.h"
#include "json_rpc.h"
#include "lsp.h"

char g_log_buf[LOG_BUFFER_SIZE];

void json_rpc_log(const char * p_message)
{
    json_t * p_args = json_object();
    json_object_set_new(p_args, "type", json_integer(MESSAGE_TYPE_LOG));
    json_object_set_new(p_args, "message", json_string(p_message));
    json_rpc_notification_send("window/logMessage", p_args);
}