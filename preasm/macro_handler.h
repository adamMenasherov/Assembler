#ifndef MACRO_HANDLER_H
#define MACRO_HANDLER_H

#include "../data_stractures/linkedlist.h"
#include "../utils/tools.h"

/* Representation of a macro */
struct macro 
{
    char macro_name[31]; /* The macro name */
    char * macro_lines; /* The macro lines (in a single string seperated by '\n')*/
};


typedef enum  /* The macro state */
{
    already_defined,
    invalid_name,
    valid
} macro_type;

struct macro_sum /* dynamic string array for macro names */
{
    char **macros;
    size_t macro_amount;
};

typedef struct macro * Macro; /* pointer to a macro object */
/**
 * @brief This function conducts a search in the macro list to find if a macro exists (comparison by macro name).
 * 
 * @param macro_list The macro linked list.
 * @param macro_name The macro name.
 * @return returns a macro object if a match was found or NULL otherwise. 
 */
Macro search_macro(LinkedList macro_list, const char *macro_name);
/**
 * @brief This function creates a macro object with a name.
 * 
 * @param macro_name The macro name.
 * @return returns a macro object if creation successful or NULL otherwise.
 */
Macro macro_creator(const char *macro_name);
/**
 * @brief This function destroyes a macro.
 * 
 * @param macro pointer to a macro pointer to be destroyed.
 */
void macro_dtor_for_linked_list(Macro * macro);
/**
 * @brief This function adds new line to macro object. 
 * 
 * @param macro_list The macro list to find the macro object.
 * @param macro_name The macro name to find the macro.
 * @param line Line to add to macro lines.
 * @return returns 1 if line addition successful, 0 otherwise. 
 */
int add_line_to_macro(LinkedList macro_list ,char *macro_name, char *line);
/**
 * @brief A constructor for a macro object (creates a deep copy)
 * 
 * @param macro macro to be copied
 * @return macro copy if copying successful, NULl otherwise.
 */
Macro macro_ctor_for_linked_list(const void * macro);
/**
 * @brief This function checks if the macro name is valid.
 * 
 * @param macro_name macro name to check.
 * @param macro_list macro list to check if macro already exists.
 * @return returns invalid_name if macro name is invalid, already_defined if macro is already defined and valid
 * if macro name is valid.
 */
macro_type check_macro_name(const char * macro_name, LinkedList macro_list);
/**
 * @brief This function copies all macro names from macro list to the macro names string array.
 * 
 * @param macro_list the macro list.
 * @param macro_names pointer to the macro names struct.
 */
void macr_names_cpy(LinkedList macro_list, struct macro_sum * macro_names);
void macro_names_destroyer(struct macro_sum * macros);

#endif