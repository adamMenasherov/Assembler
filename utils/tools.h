#ifndef TOOLS_H
#define TOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "../utils/errors.h"

#define LINE_CHECK_BUFFER_CONST 1024
#define OPCODE_AMOUNT 16
#define NEXT_NULL -1
#define INSTRUCTION_AMOUNT 4
#define REGISTER_COUNT 8
#define SKIP_SPACES(t) while((t) != NULL && isspace(*(t))) (t)++
#define WHITE_CHARS " \n\t\v\r"
#define LINE_CLOSERS "\n\r"
#define MAX_LINE_LENGTH 81
#define MAX_LABEL_LENGTH 31
#define REMOVE_FILE(file_name, error_identified) if (!error_identified) { remove(file_name); error_identified = 1; }
#define INST 'i'
#define DIR 'd'
#define LABEL_OP 'o'
#define LABEL_BEG 'b'
#define NUM_MAX 2048
#define NUM_MIN -2047

struct macro_sum; /* The macro sum struct */

/* Representation of an operation*/
struct opcode 
{
    char *opcode; /* The operation name */
    int op_types_allowed_original; /* The allowed original operand types */
    int op_types_allowed_destination; /* The allowed destination operand types */
};

/* Defining lists for instruction, registers and opcodes*/
extern struct opcode opcodes[];
extern const char * instructions[];
extern const char * registers[];

/**
 * @brief This function generates a file name with an extenstion
 * 
 * @param file_name the file name
 * @param end_signature the file extenstion
 * @return the final file name or NULL if file name generating failed.
 */
char *file_name_generator(char *file_name, char *end_signature);
/**
 * @brief This function checks the opcode in the opcodes array based on the operation name.
 * 
 * @param s the operation name.
 * @return the operation's index in the opcodes array, or -1 if not found.
 */
const int check_opcode(const char * s);
/**
 * @brief This function checks the instruction in the instruction array based on the instruction's name.
 * 
 * @param s the instruction name.
 * @return the instruction's index in the instructions array, or -1 if not found. 
 */
const int check_instruction(const char * s);
/**
 * @brief This function checks the register in the register array based on the registers's name.
 * 
 * @param s the register name.
 * @return the register's index in the instructions array, or -1 if not found. 
 */
const int check_register(const char * s);
/**
 * @brief This function checks if the register is a register pointer
 * 
 * @param s the string that represents the register
 * @param index index to look for in the register's array
 * @return the index of the register ptr, or -1 if not found.
 */
const int check_register_ptr(const char *s, int index);
/**
 * @brief This function checks if a string is a valid string (Meaning it's opening and closing with ")
 * 
 * @param s The string
 * @return 1 if the string is valid, 0 otherwise.
 */
const int check_string(const char *s);
/**
 * @brief This function checks if the label length is valid (under 31);
 * 
 * @param label_name the label to check its name.
 * @return 1 if the length is valid, 0 otherwise.
 */
int check_label_length(const char* label_name);
/**
 * @brief This function checks if all chars of str are alpha-numerical.
 * 
 * @param str The string.
 * @return 1 if it's the case, 0 otherwise.
 */
int check_alnum(char *str);
/**
 * @brief This function checks if str is valid integer, Given the mode (Instruction or directive).
 * 
 * @param str the string that represents the integer.
 * @param mode The mode (Instruction or Directive)
 * @return the number itself if it's valid, INT_MAX if it's an integer but out of range and INT_MIN if it's not a valid integer.
 */
int is_number(char *str, char mode);
/**
 * @brief This function checks if the label is defined also as a macro.
 * 
 * @param label the label to check.
 * @param macros The macro names array.
 * @return 1 if it's defined as a macro, 0 otherwise.
 */
int is_label_macro(char *label, struct macro_sum macros);
/**
 * @brief This function checks if str satisfies all rules of a valid label.
 * 
 * @param str the str that represents the label.
 * @param macros The macro names array.
 * @param mode The mode (Instruction or directive).
 * @return The label name if it's valid, NULL otherwise.
 */
char * is_valid_label(char * str, struct macro_sum macros, char mode);
/**
 * @brief This function handles malloc memory allocation.
 * 
 * @param amount amount of bytes to allocate.
 * @return pointer to the memory allocated, NULL if allocation failed.
 */
void * handle_malloc(size_t amount);
/**
 * @brief This function frees the error_location struct.
 * 
 * @param location pointer to error_location.
 */
void free_location(Error_location location);
/**
 * @brief This function handles freeing (Checks if pointer is not null)
 * 
 * @param ptr pointer to memory to be freed.
 */
void free_handler(void * ptr);
/**
 * @brief This function gets a vary amount of parameters. If a "%s" parameter appears, the next one is an str to a file 
 * that needs to be removed, otherwise, it's a file pointer to be closed.
 * 
 * @param amount amount of parameters.
 * @param ... different kinds of parameters as explained uptop.
 */
void close_and_free(int amount, ...);
/**
 * @brief This function handles file opening/creating.
 * 
 * @param file_name file names to be created/opened.
 * @param mode mode (Creating or opening)
 * @return FILE* a file pointer if opening or creating successful, NULl otherwise.
 */
FILE * fopen_handler(char * file_name, char * mode);
/**
 * @brief This function creates a short pointer.
 * 
 * @param num short num.
 * @return short* short pointer to the same num.
 */
short * short_ctor(short * num);
/**
 * @brief This function frees allocated memory for a short pointer.
 * 
 * @param num short pointer.
 */
void short_dtor(short * num);
/**
 * @brief This function hanldles file closing.
 * 
 * @param fp pointer to a file to be closed.
 */
void fclose_handler(FILE *fp);
/**
 * @brief This function creates a copy of src and puts it in dest.
 * 
 * @param dest pointer to a string.
 * @param src string to be copied to dest.
 * @return char* string copy if copying successful, NULL otherwise.
 */
char * my_strdup(char ** dest, char * src);
/**
 * @brief This function converts short to octal.
 * 
 * @param num_ptr short pointer to short num.
 * @param octal string that represents an octal number.
 */
void short_to_octal(short *num_ptr, char *octal);
/**
 * @brief This function traverses the file and checks if all lines are under 81 chars.
 * 
 * @param fp Pointer to a file.
 * @param file_name the file name. 
 * @return 1 if all lines are valid, 0 otherwise.
 */
int check_lines_length(FILE *fp, char * file_name);
/**
 * @brief This function gets a varying number of parameters and checks if any of them are null.
 * 
 * @param cnt amount of parameters.
 * @param ... varying parameters (all pointers).
 * @return 1 if any of them are null, 0 otherwise.
 */
int check_if_null(int cnt, ...);

#endif



