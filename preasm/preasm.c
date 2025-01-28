#include "../utils/tools.h"
#include "../data_stractures/linkedlist.h"
#include "../preasm/macro_handler.h"
#include "../preasm/preasm.h"
#include "../utils/errors.h"
#include <string.h>
#include <ctype.h>


line_type endmacr_case_def_or_call(char *line, LinkedList macro_list, Macro * macro_ptr) 
{
    line_type state;
    macro_call_status status;
    status = check_macro_call(line, macro_ptr, macro_list);

    if (status == macro_not_exist) /* endmacr with additional chars*/
    {
        /* Checks the macro case */
        if ((state = macro_case(line, macro_list, macro_ptr)) == bad_macro_call || state == bad_macro_defintion || state == macro_additional_line)  
        {
            return bad_endmacr_line;
        }

        else 
        {
            return state;
        }
    }

    else if (status == macro_call_with_additional_chars) /* A bad macro call was identified */ 
    {
        return bad_macro_call;
    }
    return macro_call; /* A valid macro call is identified */
}

line_type endmacr_case(char *line, LinkedList macro_list, Macro * macro_ptr) 
{
    char *temp;
    temp = strstr(line, "endmacr");

    if (temp != line) /* Potential macro call */
    {
        return endmacr_case_def_or_call(line, macro_list, macro_ptr);
    }

    /* Skipping endmacr keyword */
    temp += 7;
    SKIP_SPACES(temp); 
    
    if (*temp != '\0') /* Chars after endmacr*/
    {
        return endmacr_case_def_or_call(line, macro_list, macro_ptr);
    }

    if (*macro_ptr == NULL) /* Endmacr line without a current macro definition */
    {
        return bad_endmacr_line;
    }

    return endmacr_line; /* Valid endmacr line */
}


line_type macro_case(char *line, LinkedList macro_list, Macro * macro_ptr) 
{
    Macro new_macr;
    macro_call_status status;
    char *temp, *c1, *c2;
    const char *macro_name;
    macro_type type;

    temp = strstr(line, "macr");
    status = check_macro_call(line, macro_ptr, macro_list); /* Checking for macro call */

    if (status == valid_macro_call) /* Valid macro call was found */
    {
        return macro_call;
    }

    else if (status == macro_call_with_additional_chars) /* Macro call with additional chars */
    {
        return bad_macro_call;
    }

    if ((status = check_macro_call(line, macro_ptr, macro_list)) == macro_not_exist) 
    {
        if (*macro_ptr != NULL) /* In a macro definition */
        {
            return macro_additional_line;
        }
    }

    temp += 4; /* Skipping macr keyword */
    if (isspace(*temp)) 
    {
        /* Case 2: Potential macro defintion */
        SKIP_SPACES(temp);
        if (*temp == '\0') 
        {
            return bad_macro_defintion; /* only keyword macr in line.. */
        }

        c1 = strpbrk(temp, WHITE_CHARS);
        if (c1) 
        {
            c2 = c1;
            SKIP_SPACES(c2);
            if (*c2 != '\0') 
            {
                return bad_macro_defintion; /* Additional chars in macro definition*/
            }

            *c1 = '\0';
            macro_name = temp;

            if (!check_label_length(macro_name))  /* Macro name too long */
            {
                return bad_macro_defintion;
            }

            type = check_macro_name(macro_name, macro_list); /* Checking for macro name state */

            if (type == valid) /* new macro is valid */
            {   
                new_macr = macro_creator(macro_name); /* Creating macro */

                if (new_macr == NULL) /* Macro creation failed */
                {
                    return internal_error;
                }
                *macro_ptr = new_macr; /* Setting macro pointer to new macro */
                return macro_definition;
            }

            else  
            {
                if (type == already_defined) /* Macro is already defined */
                {
                    printf("Macro '%s' is already defined\n", macro_name);
                }

                return bad_macro_defintion;
            } 
    
        }
    }

    return any_other_line; /* It's any other line */
}



line_type line_analyzer(char *line, LinkedList macro_list, Macro * macro_ptr) 
{
    char *temp;
    macro_call_status status;

    if (strchr(LINE_CLOSERS, *line) || *line == ';') 
    {
        return blank_line;
    }

    SKIP_SPACES(line);

    if ((temp = strstr(line, "endmacr")) != NULL) /* sub-string endmacr is identified */
    {
        return endmacr_case(line, macro_list, macro_ptr);
    }

    if ((temp = strstr(line, "macr")) != NULL) /* sub-string macr is identified */
    {
       return macro_case(line, macro_list, macro_ptr);
    }

    /* final cases, macro call or any other line*/
    if ((status = check_macro_call(line, macro_ptr, macro_list)) == macro_not_exist) 
    {
        if (*macro_ptr != NULL) /* In a macro definition */
        {
            return macro_additional_line;
        }

        return any_other_line; /* it's any other line */
    }
    else if (status == macro_call_with_additional_chars) /* Exteranous chars in macro call */ 
    {
        return bad_macro_call;
    }

    return macro_call; /* valid macro call */
    
}

macro_call_status check_macro_call(char *line, Macro * macro_ptr, LinkedList macro_list) 
{
    char *temp, *c1, *macro_name;
    Macro temp_macro_ptr;
    char line_cpy[MAX_LINE_LENGTH];
    strcpy(line_cpy, line);

    temp = strpbrk(line_cpy, WHITE_CHARS);
    c1 = temp;
    if (temp != NULL) /* Seperating the macro name from the rest of the line */
    {   
        SKIP_SPACES(temp);
        *c1 = '\0';
    } 
    
    macro_name = line_cpy; /* Creating a copy */

    if ((temp_macro_ptr = search_macro(macro_list ,macro_name)) != NULL) /* There is a macro call */
    {
        if (*temp != '\0') /* Exteranous chars in macro call */
        {
            return macro_call_with_additional_chars;
        }

        *macro_ptr = temp_macro_ptr;
        return valid_macro_call; /* Macro call is valid */
    }

    if (check_macro_substring(macro_list, macro_name)) 
    {
        return macro_call_with_additional_chars; /* Additional chars in macro call */
    }

    return macro_not_exist;
}

int pre_asm_routine(char *file_name, struct macro_sum * macr_names) 
{
    LinkedList macro_linked_list;
    Node node;
    char* fp_as_file_name, *fp_am_file_name;
    FILE *fp_as_file, *fp_am_file;
    line_type state;
    int error_identified;
    char line_buffer[MAX_LINE_LENGTH], line_copy[MAX_LINE_LENGTH];
    Macro macro_ptr = NULL;
    struct error_location location;

    /* Gernerating file names, creating the macro list, an opening/creating files */
    error_identified = 0;
    fp_as_file_name = file_name_generator(file_name, ".as");
    fp_am_file_name = file_name_generator(file_name, ".am");
    macro_linked_list = new_linked_list((void *(*)(const void *))macro_ctor_for_linked_list, (void (*)(void *))macro_dtor_for_linked_list);
    location.file_name = (char *)handle_malloc((strlen(fp_as_file_name) + 1) * sizeof(char *));
    location.line = 1;

    fp_as_file = fopen_handler(fp_as_file_name, "r");
    fp_am_file = fopen_handler(fp_am_file_name, "w");

    /* Checking if anything null or if there's a too long line */
    if (check_if_null(4, fp_as_file, fp_am_file, location.file_name, macro_linked_list)|| !check_lines_length(fp_as_file, fp_as_file_name)) 
    {
        /* Deleting .am file */
        close_and_free(4, "%s", &fp_am_file_name, "f", &fp_as_file);
        fp_am_file = NULL;
        /* Cleaning memory */
        PREASM_CLEANUP(macro_linked_list, fp_as_file, fp_am_file, fp_as_file_name, fp_am_file_name, location.file_name, error_identified, macr_names);
        return 0;
    }

    printf("Entering preassembler on file %s\n", fp_as_file_name);
    strcpy(location.file_name, fp_as_file_name);
    fseek(fp_as_file, 0, SEEK_SET); /* Setting file pointer to the beginning of the file */

    while (fgets(line_buffer, sizeof(line_buffer), fp_as_file) != NULL) /* Traversing file */
    {
        strcpy(line_copy, line_buffer);

        if (strchr(line_copy, '\n') == NULL)
            line_copy[strlen(line_copy)] = '\n';

        switch(state = line_analyzer(line_copy, macro_linked_list, &macro_ptr)) /* Getting the line state */
        {
            case blank_line:
            break;
            case macro_definition:
                if (!error_identified && macro_ptr) /* No errors and macro ptr is init. */
                {
                    node = add_to_linked_list(macro_linked_list, macro_ptr); /* Adding macro to macro list. */
                    if (node == NULL) 
                    {
                        error_identified = 1;
                    }
                    macro_dtor_for_linked_list(&macro_ptr); /* Destroying temporary macro */
                    macro_ptr = node->data; /* Pointing macro pointer to new macro */
                }

                else 
                {
                    macro_dtor_for_linked_list(&macro_ptr); /* Destroying temporary macro */
                }
            break;
            case bad_macro_defintion:
                print_external_error(ERROR_CODE_2, location);
                REMOVE_FILE(fp_am_file_name, error_identified); /* Removing file */
            break;
            case endmacr_line:
                macro_ptr = NULL; 
            break;
            case bad_endmacr_line:
                print_external_error(ERROR_CODE_0, location);
                REMOVE_FILE(fp_am_file_name, error_identified); /* Removing file */
            break;
            case macro_call:
                if (!error_identified && macro_ptr->macro_lines != NULL) 
                {
                    fputs(macro_ptr->macro_lines, fp_am_file); /* Adding macro lines to .am */
                }
                macro_ptr = NULL;
            break;
            case bad_macro_call:
                print_external_error(ERROR_CODE_1, location);
                REMOVE_FILE(fp_am_file_name, error_identified); /* Removing file */
            break;
            case any_other_line:
                if (!error_identified) 
                {
                    fputs(line_buffer, fp_am_file); /* Writing line in .am */
                }
            break;
            case macro_additional_line:
                if (!error_identified && macro_ptr) 
                {
                    add_line_to_macro(macro_linked_list, macro_ptr->macro_name, line_buffer); /* Adding line to macro lines */
                }
            break;
            default:
            break;
        }

        location.line++;
    }
    
    /* Cleaning memory up and copying macro names to macro names array */
    macr_names_cpy(macro_linked_list, macr_names);
    PREASM_CLEANUP(macro_linked_list, fp_as_file, fp_am_file, fp_as_file_name, fp_am_file_name, location.file_name, error_identified, macr_names);
    return !error_identified;
}


int check_macro_substring(LinkedList macro_list, char * line) 
{
    Node p;
    if (macro_list == NULL || macro_list->lst == NULL || line == NULL) /* Checking if anything null */
    {
        return 0;
    }

    for (p = macro_list->lst; p != NULL; p = p->NextNode) /* Traversing list */
    {
        if (p->data != NULL) 
        {
            Macro macro = (Macro)(p->data);

            if (macro->macro_name != NULL) 
            {
                if (strstr(line, macro->macro_name) != NULL) /* Macro name is a sub-string of line */
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}