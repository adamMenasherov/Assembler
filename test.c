#include "./lexer/lexer.h"
#include "./test.h"


int test() 
{
    char buf[1024] = "startLabel:  mov r1, *r2";
    struct line_seperated sep = {0};
    int result = line_seperator(&sep, buf);

    return result;
}


