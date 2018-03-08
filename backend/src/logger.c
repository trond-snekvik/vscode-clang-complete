
#include <stdio.h>
#include <string.h>

int main(unsigned argc, const char ** pp_argv)
{
    while (1)
    {
        char buffer[1024];
        unsigned length;
        if (scanf("Content-Length: %u\r\n\r\n", &length))
        {
            fread(buffer, 1, length, stdin);
            FILE * file = fopen("log.txt", "a");
            fwrite(buffer, 1, length, file);
            fwrite("\r\n", 1, 2, file);
            fclose(file);
        }
    }
}