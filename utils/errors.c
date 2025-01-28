#include "../utils/errors.h"
#include <stdio.h>

/* Error codes with messages array */
struct Error errors[] = 
{
    {ERROR_CODE_0, "Endmacr line is invalid"},
    {ERROR_CODE_1, "Macro call is invalid"},
    {ERROR_CODE_2, "Macro definition is invalid"},
    {ERROR_CODE_3, "Malloc failed"},
    {ERROR_CODE_4, "Generating file name failed"},
    {ERROR_CODE_5, "Realloc failed"}
};


void print_external_error(Error_Code error_code, struct error_location location) 
{
    printf("Error: %s in %s:%d\n", errors[error_code].error_message , location.file_name, location.line); /* Printing the external error */
}


void print_internal_error(Error_Code error_code) 
{
    printf("Error: %s\n", errors[error_code].error_message); /* Printing the internal erro */
}