#include "../preasm/macro_handler.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../data_stractures/linkedlist.h"

Macro macro_creator(const char *macro_name) 
{
    Macro new_macr = (Macro)handle_malloc(sizeof(struct macro)); /* Memory allocation for macro */

    if (new_macr == NULL) /* Checking if allocation failed */
    {
        return NULL;
    }

    strcpy(new_macr->macro_name, macro_name); /* Copying macro name */
    new_macr->macro_lines = NULL;

    return new_macr;
}

void macro_dtor_for_linked_list(Macro * macro) 
{
    free((*macro)->macro_lines); /* Destroying macro lines */
    free(*macro); /* Destroying macro*/
    *macro = NULL; /* Setting macro pointer to null */
}

int add_line_to_macro(LinkedList macro_list ,char *macro_name, char *line) 
{
    size_t original_macro_lines_length, new_line_length, total_length;
    Macro macro; 
    char * ptr;
    macro = search_macro(macro_list, macro_name); /* Searching for macro object */

    if (macro == NULL) 
    {
        return -1;
    }
    
    /* Calculating new necessary memory */
    original_macro_lines_length = macro->macro_lines ? strlen(macro->macro_lines) : 0;  
    new_line_length = strlen(line);
    total_length = (original_macro_lines_length + new_line_length + 1) * sizeof(char);

    ptr = (char *)realloc(macro->macro_lines, total_length); /* Reallocating for necessary memory for line addition */

    if (ptr == NULL) /* Reallocation failed */
    {
        print_internal_error(ERROR_CODE_5);
        return 0;
    }

    memset((ptr + original_macro_lines_length), 0, new_line_length + 1); /* Setting new chars to 0*/
    macro->macro_lines = ptr;

    strncat(macro->macro_lines, line, new_line_length); /* Concatenating new line to macro lines*/
    return 1;
}


Macro search_macro(LinkedList macro_list, const char *macro_name) 
{
    Node p;
    if (macro_list == NULL || macro_list->lst == NULL || macro_name == NULL) /* Checking if anything null */
    {
        return NULL;
    }

    for (p = macro_list->lst; p != NULL; p = p->NextNode) /* Traversing list */
    {
        if (p->data != NULL) 
        {
            Macro macro = (Macro)(p->data);

            if (macro->macro_name != NULL) 
            {
                if (strcmp(macro->macro_name, macro_name) == 0) /* Comapring names */
                {
                    return p->data; /* Returning macro object */
                }
            }
        }
    }

    return NULL; /* No match was found */
}


Macro macro_ctor_for_linked_list(const void * macro) 
{
    Macro new_macr = (Macro)handle_malloc(sizeof(struct macro)); /* Allocating memory for macro object */
    if (new_macr == NULL) 
    {
        return NULL;
    }

    new_macr->macro_lines = NULL;
    return memcpy(new_macr, macro, sizeof(struct macro)); /* Creating a deep copy */
}


macro_type check_macro_name(const char * macro_name, LinkedList macro_list) 
{
    /* Checking if macro name is endmacr, macr or if it starts with a non-alpha char */
    if (!isalpha(*macro_name) || strcmp(macro_name, "endmacr") == 0 || strcmp(macro_name, "macr") == 0)  
    {
        return invalid_name;
    }

    /* checking if it's a reserved keyword */
    if (check_opcode(macro_name) == -1 && check_instruction(macro_name) == -1 && check_register(macro_name) == -1) 
    {
        if (search_macro(macro_list, macro_name) == NULL)
        {
            return valid; /* Macro name is valid */
        }

        return already_defined; /* Macro is already defined */
    }

    return invalid_name; /* Macro is a reserved keyword */
}

void macr_names_cpy(LinkedList macro_list, struct macro_sum * macro_names) 
{
    int i;
    size_t lst_size;
    Node p;

    lst_size = get_size(macro_list); /* Getting macro list size */
    macro_names->macros = handle_malloc(lst_size * sizeof(char *)); /* Allocating memory for macro names */

    for (i = 0, p = macro_list->lst; p != NULL; p = p->NextNode, i++) /* Traversing list */
    {
        macro_names->macros[i] = handle_malloc(sizeof(char) * strlen(((Macro)(p->data))->macro_name) + 1);
        strcpy(macro_names->macros[i], ((Macro)(p->data))->macro_name); /* Copying macro names */
    }

    macro_names->macro_amount = lst_size; /* Setting macro amount to amount of macros in macro list */
}

void macro_names_destroyer(struct macro_sum * macros) 
{
    size_t i;

    if (macros == NULL) 
    {
        return;
    }

    for (i = 0; i < macros->macro_amount; i++)  /* Traversing macro names array */
    {
        free(macros->macros[i]); /* Freeing the macro name */
    }

    free(macros->macros); /* Freeing the macro names array */
}