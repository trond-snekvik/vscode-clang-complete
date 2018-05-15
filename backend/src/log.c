#include "log.h"
#include "json_rpc.h"
#include "lsp.h"
#include "utils.h"

char g_log_buf[LOG_BUFFER_SIZE];
static mutex_t m_mut;
void log_init(void)
{
    mutex_init(&m_mut);
}

void safe_puts(const char * p_filename, const char * p_string)
{
    mutex_take(&m_mut);
    FILE *f = fopen(p_filename, "a");
    if (f)
    {
        fputs(p_string, f);
        fclose(f);
    }
    mutex_release(&m_mut);
}

void json_rpc_log(const char * p_message)
{
    json_t * p_args = json_object();
    json_object_set_new(p_args, "type", json_integer(MESSAGE_TYPE_LOG));
    json_object_set_new(p_args, "message", json_string(p_message));
    json_rpc_notification_send("window/logMessage", p_args);
}