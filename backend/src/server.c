#include <signal.h>
#include "command_handler.h"
#include "json_rpc.h"
#include "log.h"
#include "path.h"


void assert_handler(const char * p_file, unsigned line)
{
    LOG("ASSERT @ %s:%u\n", p_file, line);
    fflush(stdout);
    exit(-1);
}

void signal_handler(int signal)
{
    LOG("SIGNAL: %d\n", signal);
    exit(-1);
}

int main(unsigned argc, const char ** pp_argv)
{
    signal(SIGINT, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGBREAK, signal_handler);
    signal(SIGABRT, signal_handler);
    OUT_ERASE();
    LOG("-------------------------------------------------------\n");
    LOG("Log started\n");
    LOG("-------------------------------------------------------\n");
    json_rpc_init();

    command_handler_init();
    FILE * p_stream = stdin;
    if (argc == 2)
    {
        p_stream = fopen(pp_argv[1], "r");
        ASSERT(p_stream);
        LOG("Running from file %s\n", pp_argv[1]);
    }
    json_rpc_listen(p_stream);

    LOG("Stopped listening.\n");
    LOG("Closing.\n");
}

