#ifndef ERRORS_H
#define ERRORS_H

/* Representation of error */
struct Error 
{
    int error_code; /* The error code */
    char *error_message; /* The error message */
};


struct error_location  /* Error location representation */
{
    char *file_name; /* File name */
    int line; /* Line of error */
};

typedef struct error_location * Error_location; /* Pointer to error_location */

typedef enum  /* Error Codes enum*/
{
    ERROR_CODE_0,
    ERROR_CODE_1,
    ERROR_CODE_2,
    ERROR_CODE_3,
    ERROR_CODE_4,
    ERROR_CODE_5
} Error_Code;

/**
 * @brief This function prints an external error to stdout.
 * 
 * @param error_code the error code.
 * @param location the location of the error.
 */
void print_external_error(Error_Code error_code, struct error_location location);
/**
 * @brief This function prints an internal error to stdout.
 * 
 * @param error_code the error code.
 */
void print_internal_error(Error_Code error_code);


#endif