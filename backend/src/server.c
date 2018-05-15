#include <signal.h>
#include "command_handler.h"
#include "json_rpc.h"
#include "log.h"
#include "path.h"
#include "unit_storage.h"
#include "unsaved_files.h"


void assert_handler(const char * p_file, unsigned line)
{
    LOG("ASSERT @ %s:%u\n", p_file, line);
    fprintf(stderr, "ASSERT @ %s:%u\n", p_file, line);
    fflush(stdout);
    while(1);
    exit(-1);
}

void signal_handler(int signal)
{
    LOG("SIGNAL: %d\n", signal);
    fprintf(stderr, "SIGNAL: %d\n", signal);
    while(1);
    exit(-1);
}

BOOL WINAPI ctrl_handler(DWORD ctrl_type)
{
	LOG("CTRL: %#x\n", ctrl_type);
	return true;
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
	SetConsoleCtrlHandler(ctrl_handler, true);
    log_init();
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
	unit_storage_wait_for_completion();
    unsaved_files_free();
    unit_index_free();

    LOG("Stopped listening.\n");
    LOG("Closing.\n");
}

