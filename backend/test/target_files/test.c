#include <stdio.h>

typedef void (*function_type_t)(int a, int b);

typedef enum
{
    LOL_A,
    LOL_B
} lol_t;

typedef struct
{
    int a;
    struct
    {
        int a;
        float b;
        int c;
    } substruct;
    float f;
    /** hello */
    double d;
    union
    {
        int a;
        float b;
    } u;
    void (*b)(int a); /* yes */
    function_type_t func; /**< Hello dude */
    lol_t l;
} struct_t;

#define MY_MACRO(a, b) printf("%s, %s\n", a, b)
#define MY_DEFINE 5
/**
 * Completion is fun
 *
 * \param a This parameter isn't really used
 * \param b Neither is this one
 */
int my_function(int a, int b)
{
    return printf("in my function\n");
}

void test_function(void)
{
    struct_t s;
    MY_MACRO("lol", "lol2");
    my_function(1, 2);
    s.substruct.a = 1;
    s.substruct.b = 0.1f;
}