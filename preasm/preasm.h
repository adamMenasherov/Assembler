#ifndef PREASM_H
#define PREASM_H

#include "../utils/tools.h"
#include "../data_stractures/linkedlist.h"
#include "../preasm/macro_handler.h"

/* Preassembler memory cleaner */
#define PREASM_CLEANUP(macro_linked_list, fp_as_file, fp_am_file, fp_as_file_name, fp_am_file_name, location_file_name, error_identified, macr_names) \
    linked_list_destroyer(&(macro_linked_list)); \
    fclose_handler(fp_as_file); \
    fclose_handler(fp_am_file); \
    free_handler(fp_as_file_name); \
    free_handler(fp_am_file_name); \
    free_handler(location_file_name); \
    if (error_identified) macro_names_destroyer(macr_names); \


/* Enum for line state */
typedef enum  
{
    blank_line,
    macro_definition, 
    bad_macro_defintion, 
    endmacr_line,
    bad_endmacr_line,
    macro_call,
    bad_macro_call,
    any_other_line,
    macro_additional_line,
    internal_error
} line_type;

typedef enum /* Enum for macro call state */
{
    macro_not_exist, 
    macro_call_with_additional_chars,
    valid_macro_call
} macro_call_status;


/**
 * @brief This function handles the case where the sub-string "endmacr" appeares in line.
 * 
 * @param line Line from the file.
 * @param macro_list The macro linked list.
 * @param macro_ptr Macro pointer to check if there's a current macro definition.
 * @return The function returns the line type.
 */
line_type endmacr_case(char *line, LinkedList macro_list, Macro * macro_ptr);
/**
 * @brief This function handles the case where the sub-string "macr" appeares in line.
 * 
 * @param line Line from the file.
 * @param macro_list The macro linked list.
 * @param macro_ptr Macro pointer to check if there's a current macro definition.
 * @return The function returns the line type.
 */
line_type macro_case(char *line, LinkedList macro_list, Macro * macro_ptr);
/**
 * @brief This function analyzes the line type.
 * 
 * @param line Line from the file.
 * @param macro_list The macro linked list.
 * @param macro_ptr Macro pointer to check if there's a current macro definition.
 * @return The function returns the line type.
 */
line_type line_analyzer(char *line, LinkedList macro_list, Macro * macro_ptr);
/**
 * @brief The function returns a state of macro call. 
 * 
 * @param line Line from the file.
 * @param macro_list The macro linked list.
 * @param macro_ptr Macro pointer to check if there's a current macro definition.
 * @return The function returns the final state of the macro call.
 */
macro_call_status check_macro_call(char *line, Macro * macro_ptr, LinkedList macro_list);
/**
 * @brief The function checks if the macro name is valid and returns a state of validation.
 * 
 * @param macro_name macro name to have the checks on.
 * @param macro_list The macro linked list to check if macro is already defined.
 * @return Returns the state of validation.
 */
macro_type check_macro_name(const char * macro_name, LinkedList macro_list);
/**
 * @brief This function handles the entire preassembler stage routine. It handles all the necessary checks, and 
 * creates .am file if no errors have occured.
 * 
 * @param file_name the file name.
 * @param macr_names pointer to the macro names array to manipulate.
 * @return 1 if preassembler routine was successful, 0 otherwise.
 */
int pre_asm_routine(char *file_name, struct macro_sum  * macr_names);
/**
 * @brief This function hanldes special cases of endmacr line (such as macro call of definition).
 * 
 * @param line Line from the file.
 * @param macro_list The macro linked list.
 * @param macro_ptr The macro pointer to check if there is a current macro definition.
 * @return the line state. 
 */
line_type endmacr_case_def_or_call(char *line, LinkedList macro_list, Macro * macro_ptr);
/**
 * @brief This function checks if an existing macro is a sub-string of the line. 
 * 
 * @param macro_list the macro linked list. 
 * @param line Line from the file.
 * @return 1 if it is the case, 0 otherwise.
 */
int check_macro_substring(LinkedList macro_list, char * line);


#endif