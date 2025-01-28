#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "utils_first_pass.h"
#include "../lexer/lexer.h"
#include "../preasm/macro_handler.h"

#define MAX_MEMORY_SIZE 4096
#define OPCODE_MOVE 11
#define ADDRESS_MOVE_ORIG 7
#define ADDRESS_MOVE_DEST 3
#define REGISTER_MOVE_ORIG 6
#define REGISTER_MOVE_DEST 3
#define A 2
#define R 1
#define IMMEDIATE_MOVE 3
#define DIRECT_LABEL_MOVE 3
#define OPERAND_MAX 2
#define OCTAL_LENGTH 7

enum 
{
    MULTIPLE_DEF,
    FOUND_ENTRY,
    NOT_FOUND
};

/* Macro for inserting element to a list (word) */
#define INSERT_ELEMENT(arr, status, next_word, i)                     \
    status = insert_new_element(arr, &(next_word), i, 0); \
    if (status == NULL)                                             \
    {                                                               \
        return 0;                                                   \
    }                                                               \
    i++;                                                   \
    (next_word) = 0;

/* Macro for inserting symbol to a symbol table */
#define INSERT_SYMBOL(arr, status, item, i, flag)  \
    status = insert_new_element(arr, item, i, flag); \
    if (status == NULL) { \
        return 0; \
    } \

/* Error message printer (by file name + line )*/
#define PRINT_ERROR_MESSAGE(lex, location) \
    if (is_error_message_init(lex)) { \
        printf("Error: %s in %s:%d\n", \
               (lex)->error_message, \
               (location)->file_name, \
               (location)->line); \
    }


/* Memory cleanup for first pass */
#define CLEAN_UP_FIRST_PASS(final_file_name, unit, location, sep, lex, fp, macros) \
    free_handler(final_file_name);                             \
    free_location(location);                                   \
    free_line_seperator(sep);                                  \
    free_handler(lex);                                         \
    fclose_handler(fp);                                        \
    macro_names_destroyer(macros);

/* Memory cleanup for final files creator */
#define CLEAN_UP_FILE_CREATOR(fp_ent_file, fp_ob_file, fp_ext_file, file_name_ext, file_name_ob, file_name_ent) \
    fclose_handler(fp_ent_file); \
    fclose_handler(fp_ext_file); \
    fclose_handler(fp_ob_file); \
    free_handler(file_name_ext); \
    free_handler(file_name_ent); \
    free_handler(file_name_ob); \



/**
 * @brief This function handles the case of a direct instruction. 
 * 
 * @param lex pointer to line_lexer.
 * @param unit pointer to translation_unit.
 * @return 1 if convertion successful, 0 otherwise.
 */
int dir_convertion(Line_lexer lex, Translation_unit unit);
/**
 * @brief This function handles the case of a instruction line. 
 * 
 * @param lex pointer to line_lexer.
 * @param unit pointer to translation_unit.
 * @return 1 if convertion successful, 0 otherwise.
 */
int inst_convertion(Line_lexer lex, Translation_unit unit);
/**
 * @brief This function handles the entire first pass stage. It creates the encoding for the code image and the data image 
 * and the symbol table, and also goes to the second pass if no errors occured in the first.
 * 
 * @param file_name the name of the file to have the first pass on.
 * @param macros pointer to macro_sum. 
 */
void first_pass(char *file_name, struct macro_sum * macros);
/**
 * @brief This function checks for any other definitions of a certain symbol.
 * 
 * @param unit pointer to the translation_unit.
 * @param sym symbol to have the checks on.
 * @param lex pointer to line_lexer.
 * @param state symbol state.
 * @param address symbol address.
 * @return the function returns MULTIPLE_DEF if the label has already been defined, FOUND_ENTRY if the symbol was declared as entry in the symbol table
 * and NOT_FOUND if the symbol was not found at all in the symbol table.
 */
int check_for_definitions(Translation_unit unit, Symbol sym, Line_lexer lex, enum symbol_state state, int address);
/**
 * @brief This function updates all the addresses in the code and data images.
 * 
 * @param unit pointer to the translation unit.
 */
void change_label_address(Translation_unit unit);
/**
 * @brief This function handles the entire second pass. It inserts the words or direct labels in the code image.
 * 
 * @param file_name the file name to have the second pass on. 
 * @param fp Pointer to the file. 
 * @param unit pointer to the translation_unit.
 * @param lex pointer to line_lexer.
 * @param sep pointer to line_seperated.
 * @param macros pointer to macro_sum.
 * @param location pointer to error_location.
 * @return returns 1 if second pass was successfull and 0 otherwise.
 */
int second_pass(char * file_name, FILE * fp ,Translation_unit unit, Line_lexer lex, Line_seperated sep, struct macro_sum * macros, Error_location location);
/**
 * @brief This function handles the encoding of the direct labels in instruction lines in code image, and updates IC and DC counters.
 * 
 * @param unit pointer to the translation_unit.
 * @param lex pointer to line_lexer.
 * @return returns 1 if convertion was successful, 0 otherwise.
 */
int second_pass_main(Translation_unit unit ,Line_lexer lex);
/**
 * @brief This function creates the final files. Object, Extern and Entry given the situation.
 * 
 * @param unit pointer to the translation unit.
 * @param file_name file name as an initial to the other files to be created.
 * @return 1 if file creation was successful, 0 otherwise.
 */
int create_final_files(Translation_unit unit, char * file_name);



#endif