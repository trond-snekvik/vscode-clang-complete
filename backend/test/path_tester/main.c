#include "path.h"
#include <stdio.h>

void assert_handler(const char * p_file, unsigned line)
{
    printf("ASSERT @ %s:%u\n", p_file, line);
    fflush(stdout);
    exit(1);
}

void test(char * p_path)
{
    char * p_fixed = path_remove_redundant_steps(p_path);
    printf("old:\t%s\nnow:\t%s\n", p_path, p_fixed);
}

void main(void)
{
    test("C:/Users/Trond/Documents");
    test("C:/Users/Trond/Documents/..");
    test("C:/Users/Trond/Documents/../../heisann");
    test("C:/Users/Trond/Documents/../../heisann/main.c");
    test("top/subdir/subsub/../other");
    test("./top/subdir/subsub/../other");
}