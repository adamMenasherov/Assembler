#include "../utils/tools.h"
#include "../preasm/macro_handler.h"
#include "../utils/errors.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>

/* Opcode array*/
struct opcode opcodes[] = 
{
    {"mov", 15, 14},
    {"cmp", 15, 15},
    {"add", 15, 14},
    {"sub", 15, 14},
    {"lea", 2, 14},
    {"clr", 0, 14},
    {"not", 0, 14}, 
    {"inc", 0, 14},
    {"dec", 0, 14},
    {"jmp", 0, 6},
    {"bne", 0, 6},
    {"red", 0, 14},
    {"prn", 0, 15},
    {"jsr", 0, 6},
    {"rts", 0, 0},
    {"stop", 0, 0}
};

/* Instruction and register arrays */
const char * instructions[] = {".data", ".string", ".entry", ".extern"};
const char * registers[] = {"r0", "r1", "r2", "r3", "r4" ,"r5", "r6", "r7"};

char *file_name_generator(char *file_name, char *end_signature) 
{
    char *final_file_name = handle_malloc(sizeof(char) * (strlen(file_name) + strlen(end_signature)) + 1); /* Allocating memory */

    if (final_file_name == NULL) /* Allocation failed */
    {
        print_internal_error(ERROR_CODE_4);
        return NULL;
    }

    strcpy(final_file_name, file_name); /* Copying file name */
    strcat(final_file_name, end_signature); /* Concatenating file signature */

    return final_file_name; /* Returning final file name */
}


const int check_opcode(const char * s) 
{
    int i;

    for (i = 0; i < OPCODE_AMOUNT; i++) 
    {
        if (strcmp(s, opcodes[i].opcode) == 0)  /* Checking if opcode found */
        {
            return i;
        }
    }

    return -1;
}


const int check_instruction(const char *s) 
{
    int i;

    for (i = 0; i < INSTRUCTION_AMOUNT; i++) 
    {
        if (strcmp(s, instructions[i]) == 0)  /* Checking if instruction found */
        {
            return i;
        }
    }

    return -1;
}

const int check_register(const char * s) 
{
    int i;

    for (i = 0; i < REGISTER_COUNT; i++) 
    {
        if (strstr(s, registers[i]) != NULL) /* Checking if register found */
        {
            if (strcmp(s, registers[i]) == 0 || check_register_ptr(s, i)) /* Checking if it's a register direct or pointer */
            {
                return i;
            }
        }
    }

    return -1; /* None of them was found */
}

const int check_register_ptr(const char *s, int index) 
{
    char *temp;

    if ((temp = strstr(s, "*r")) != NULL) 
    {
        if (temp == s) 
        {
            temp += 1;
            if (strcmp(temp, registers[index]) == 0) /* Checking if register number is valid */
            {
                return 1;
            }
        }
    }

    return 0; /* Not a register ptr */
}


int check_label_length(const char* label_name) 
{
    if (strlen(label_name) < MAX_LABEL_LENGTH) /* Label length is valid*/
    {
        return 1;
    }

    return 0; /* Invalid label length */
}

int check_alnum(char *str) 
{
    while (*str != '\0') /* Traversing str */
    {
        if (!isalnum(*str)) 
        {
            return 0; /* Char is not alpha-numerical */
        }
        str++;
    }
    return 1;
}


char * is_valid_label(char * str, struct macro_sum macros, char mode) 
{ 
    char *temp, *label_name;
    label_name = (char *)handle_malloc(sizeof(char) * strlen(str) + 1); /* Allocating memory */

    if (label_name == NULL)  /* Allocating failed */
    {
        return NULL;
    }

    strcpy(label_name, str); /* Copying label name */

    if (is_label_macro(str, macros)) /* Checking if label is macro name */
    {
        free(label_name);
        return NULL;
    }

    if (!check_label_length(str)) /* Checking if label length is valid */
    {
        free(label_name);
        return NULL;
    }

    if (!isalpha(*str) || check_opcode(str) >= 0 || check_instruction(str) >= 0 || check_register(str) >= 0) /* Label name is invalid */
    {
        free(label_name);
        return NULL;
    } 

    if (((temp = strchr(label_name, ':')) != NULL)) 
    {
        if (mode == LABEL_BEG) /* Removing char : */
        {
            *temp = '\0';
        }
        
        else 
        {
            free(label_name);
            return NULL;
        }
    }

    if (!check_alnum(label_name)) /* Checking if all chars are alpha-numerical */
    {
        free(label_name);
        return NULL;
    }

    return label_name;
}

int is_number(char *str, char mode) 
{
    char *end_ptr;
    int final_num;

    if (*str != '#') 
    {
        if (mode == INST) 
        {
            return INT_MIN; /* Integer is not valid */
        }
        
    }

    else 
    {
        if (mode != INST) 
        {
            return INT_MIN; /* Integer is not valid */
        }
        str++;
    }

    final_num = (int)strtol(str, &end_ptr, 10); /* Getting final integer */

    if (*end_ptr != '\0' || end_ptr == str) 
    {
        return INT_MIN; /* Integer is not valid */
    }

    if (final_num > NUM_MAX || final_num < NUM_MIN) 
    {
        return INT_MAX; /* Integer is out of range */
    }

    return final_num; /* Integer is valid */
}

int is_label_macro(char *label, struct macro_sum macros) 
{
    int i;

    for (i = 0; i < macros.macro_amount; i++) 
    {
        if (strcmp(label, macros.macros[i]) == 0) /* Label is defined as macro */
        {
            return 1;
        }
    }

    return 0;
}

const int check_string(const char *s) 
{
    return *s == *(s + strlen(s) - 1)  && *s == '\"'; /* Checking if both first char and end char are \" "*/
}

void * handle_malloc(size_t amount) 
{
    void * ptr;
    ptr = malloc(amount); /* Allocating memory */

    if (ptr == NULL) 
    {
        print_internal_error(ERROR_CODE_3); /* Allocation failed */
    }

    return ptr;
}

void free_location(Error_location location) 
{
    if (location->file_name != NULL) 
    {
        free(location->file_name); /* Freeing file name */
    }
    free(location); /* Freeing struct */
}

void close_and_free(int amount, ...) 
{
    int i;
    char **str;
    FILE **fp;
    va_list args;
    va_start(args, amount);
    for (i = 0; i < amount; i++) /* Traversing operands */
    {
        if (strcmp(va_arg(args, char*), "%s") == 0) /* File removal and string free */
        {
            i++;
            str = va_arg(args, char**);
            remove(*str); /* Removing file */
            free(*str);
            *str = NULL;
        }

        else /* File closing */
        {
            i++;
            fp = va_arg(args, FILE **);
            if (fp != NULL && *fp != NULL) 
            {
                fclose_handler(*fp);
                *fp = NULL;
            }
        }
    }
    va_end(args);
}

FILE * fopen_handler(char * file_name, char * mode) 
{
    FILE * fp;
    char error_mode[20];

    if (file_name == NULL) 
    {
        return NULL;
    }

    fp = fopen(file_name, mode);

    if (fp == NULL) /* Couldn't create/open file */
    {
        (strcmp(mode, "w") == 0) ? strcpy(error_mode, "create") : strcpy(error_mode, "open");
        printf("Error: Couldn't %s file %s\n", error_mode, file_name); /* Creation/opening failed */
    }

    return fp; /* Returning file pointer */
}

short * short_ctor(short * num) 
{
    short * new_int;

    new_int = (short *)handle_malloc(sizeof(short)); /* Allocating memory */

    if (new_int == NULL) /* Check for allocation */
    {
        return NULL;
    }

    new_int = memcpy(new_int, num, sizeof(short)); /* Copying word (short number )*/
    return new_int;
}

int check_if_null(int cnt, ...) 
{
    int i;
    va_list args;

    va_start(args, cnt);

    for (i = 0; i < cnt; i++) 
    {
        if (va_arg(args, void *) == NULL) /* Checking if pointer is null */
        {
            return 1;
        }
    }

    return 0; /* Nothing is null */
}

void short_dtor(short * num) 
{
    free(num); /* Releasing memory */
}

void free_handler(void * ptr) 
{
    if (ptr == NULL) 
    {
        return;
    }

    free(ptr); /* Releasing memory */
}

void fclose_handler(FILE *fp) 
{
    if (fp == NULL) 
    {
        return;
    }

    fclose(fp); /* Closing file */
}

char * my_strdup(char ** dest, char * src) 
{
    if (src == NULL) 
    {
        return NULL;
    }

    *dest = (char *)(handle_malloc(strlen(src) + 1)); /* Allocating memory for string */

    if (*dest == NULL) 
    {
        return NULL;
    }

    strcpy(*dest, src); /* Copying string */
    return *dest;
}

void short_to_octal(short *num_ptr, char *octal) {
    unsigned short unum;
    if (num_ptr == NULL) {
        return;
    }
    unum = (*num_ptr) & 0x7FFF; /* Taking only 15 LSB*/
    sprintf(octal, "%05o", unum); /* Octal will have a 5 chars zero padding */
}


int check_lines_length(FILE *fp, char * file_name) 
{
    int line;
    char buffer[LINE_CHECK_BUFFER_CONST]; /* Buffer of size 1024 to check line length*/
    fseek(fp, 0, SEEK_SET);
    line = 1;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) 
    {
        if (strlen(buffer) > MAX_LINE_LENGTH) /* line length is bigger than */
        {
            printf("Line length is beyond maximum (81) in %s:%d\n", file_name, line);
            return 0;
        }
        line++;
    }

    return 1; /* All file lines length are valid */
}