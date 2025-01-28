#include "lexer/lexer.h"
#include "preasm/preasm.h"
#include "passes/firstpass.h"
#include "passes/utils_first_pass.h"
#include "utils/tools.h"
#include "data_stractures/dynamic_arr.h"
#include "data_stractures/linkedlist.h"
#include "./test.h"


int main(int argc, char ** argv) 
{
    test();
    struct macro_sum macros = {0};
    int i;


    for (i = 1; i < argc; i++) 
    {
        if (!pre_asm_routine(argv[i], &macros)) 
        {
            printf("Preassebler failed\n");
            continue;
        }

        printf("Entering first pass\n");
        first_pass(argv[i], &macros);
        putchar('\n');     
    }
    return 0;
}

