#include "lexer.h"
#include "../preasm/macro_handler.h"
#include "../utils/tools.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>


int line_seperator(Line_seperated new_seperator, char *line) 
{
    int i, equal, comma_flag;
    char *temp, c;

    if (new_seperator == NULL) 
    {
        return 0;
    }

    new_seperator->ptr_cntr = 0; /* No words has been found yet */
    SKIP_SPACES(line); /* Skipping all white chars */

    if (*line == '\0' || *line == ';') /* Empty or comment line */
    {
        return 1;
    }
    
    i = 1;
    do
    {
        c = ' ';
        comma_flag = 1;
        equal = 0;
        temp = strpbrk(line, WHITE_CHARS_COMMA); /* Checking for next comma or white char */
        if (temp == NULL) /* Last word reached */
        {
            c = '\0'; /* Seperating the last string */
            temp = &c;
            i = 0; /* Setting i = 0 to exit loop */
        }

        if (temp == line) /* Checking if line and temp equal */
        {
            equal = 1;
            temp++;
        }

        c = *temp;
        *temp = '\0';  /* Breaking line to word by putting a null terminator */
        if (*line != '\0') 
        {
            ADD_WORD_TO_SEP(line, new_seperator); /* Adding word to seperator*/

            if(c == *line) /* Making sure to not add two commas */
                comma_flag = 0;

            if (equal) 
            {
                *temp = c;
                line = temp;
            }
            else 
            {
                if (i)
                    line = temp + 1; /* Skipping to next word */
            }
            SKIP_SPACES(line);  /* Skipping white chars */
        }

        if (c == ',' && comma_flag) 
        {
            ADD_WORD_TO_SEP(",", new_seperator); /* Adding comma to seperator */
        }

    } while(i);

    return 1;
}

void free_line_seperator(Line_seperated sep) 
{
    int i;

    if (sep == NULL) 
    {
        return;
    }

    for (i = 0; i < sep->ptr_cntr; i++) 
    {
        free(sep->words_ptrs[i]); /* Freeing each word */
    }
    memset(sep, 0, sizeof(struct line_seperated)); /* Setting all string pointers to null */
}

int main_lexer(Line_lexer lex, Line_seperated sep, struct macro_sum macros, char * line) 
{
    int operand_amount;

    if (sep->ptr_cntr == 0) 
    {
        lex->line_type = empty; /* Empty line */
        return 1;
    }

    if (sep->ptr_cntr == 1) /* Ther's only one keyword in line */
    {
        if (check_opcode(sep->words_ptrs[0]) >= 0) /* Opcode has been found*/
        {
            lex->line_type = inst;
            inst_case(lex, sep, macros); /* Getting to instruction case */

            if (is_error_message_init(lex)) /* Checking for error */
            {
                return 0;
            }
            return 1;
        }

        strcpy(lex->error_message, "Invalid Keyword at the beginning of line"); /* One word line can only be instruction line */
        return 0;
    }

    if (check_opcode(sep->words_ptrs[0]) >= 0 || check_opcode(sep->words_ptrs[1]) >= 0) /* Instruction line case */
    {
        if ((operand_amount = check_how_many_operands(sep)) > MAX_OPERANDS_INST) /* Four are most operands that are available in instruction */
        {
            sprintf(lex->error_message, "%d is too many operands for instruction line", operand_amount);
            return 0;
        }

        lex->line_type = inst;
        inst_case(lex, sep, macros); /* Getting to instruction case */

        if (is_error_message_init(lex)) /* Checking for error */
        {
            return 0;
        }
        return 1;
    }

    else if (check_instruction(sep->words_ptrs[0]) >= 0 || check_instruction(sep->words_ptrs[1]) >= 0) /* Direct line case */ 
    {
        lex->line_type = dir;
        dir_case(lex, sep, macros, line); /* Getting to directive case */
        if (is_error_message_init(lex)) /* Checking for error */
        {
            return 0;
        }
        return 1;
    }
    
    /* If it's not a directive and not an instruction, it's an invalid line */
    strcpy(lex->error_message, "Not a valid line - Line includes invalid keywords");
    return 0;
}


int check_label_and_errors(Line_lexer lex, Line_seperated sep, struct macro_sum macros, char mode, int * i) 
{
    int error;
    char *label_name;
    error = 0;

    label_name = is_valid_label(sep->words_ptrs[0], macros, LABEL_BEG); /* Checking wheter there is a label at the beggining of line */

    if (label_name) /* Label at the beginning of line */
    {  
        strcpy(lex->label_name, label_name);
        free(label_name);
        *i = 1;
    }
    
    else /* Not a valid label, options are opcode or instruction */
    {
        switch(mode) 
        {
            case INST:
                if(check_opcode(sep->words_ptrs[0]) == -1) /* If it's not an opcode, there's an error */
                    error = 1;
                break;
            case DIR:
                if (check_instruction(sep->words_ptrs[0]) == -1) /* If it's not a directive instruction, there's an error */
                    error = 1;
                break;
        }
    }

    if (error) /* Not a valid keyword at beginning of line */
    {
        strcpy(lex->error_message, "Label is invalid - INVALID KEYWORD");
        return 0;
    }

    return 1;
}

void inst_case(Line_lexer lex, Line_seperated sep, struct macro_sum macros) 
{
    int opcode_i, i, j, operand, in_string;
    enum op_type type;
    char *temp;
    in_string = 0;

    operand = 0;
    i = 0;

    if (!check_label_and_errors(lex, sep, macros, INST, &i)) /* errors have occured */
        return;

    opcode_i = check_opcode(sep->words_ptrs[i]); /* Checking for opcode */
    lex->line.inst.opcode = opcode_i;

    if (!check_operand_amount_compatible_inst(lex, sep, opcode_i, &operand)) /* Operand amount is not compatible with the opcode */ 
    {
        sprintf(lex->error_message, "Operand amount doesn't match opcode %s", opcodes[opcode_i].opcode);
        return;
    } 

    if (!check_line_comma_compatible(lex, sep)) /* Checking for comma overflow */
    {
        strcpy(lex->error_message, "Comma overflow");
        return;
    }
    
    i++;

    /* Operands check */

    switch(operand) /* How many operands does the operation have */
    {
        case 1: 
            j = DESTINATION; /* Only dest operand */
            break;
        case 2:
            j = ORIGINAL; /* Both */
            break;
    }

    lex->line.inst.operand_cnt = 0; /* Setting current operand count to 0 */

    while (i < sep->ptr_cntr) /* Traversing line tokens */
    {
        type = operand_type_analyzer(sep->words_ptrs[i], macros, INST, in_string); /* Analyzing operand type */

        /* If it's an integer that's out of range and the operation can recieve an integer, the operand is invalid */
        if (type == out_of_range && check_operand_type_compatible_inst(opcode_i, immediate, j)) 
        {
            sprintf(lex->error_message, "Number %s is out of range", sep->words_ptrs[i]);
            return;
        }

        if (type == invalid || !check_operand_type_compatible_inst(opcode_i, type, j)) /* Operand is invalid or not comaptible with instruction */
        {
            sprintf(lex->error_message, "Operand type doesn't match opcode %s", opcodes[opcode_i].opcode);
            return;
        }
        j++;

        switch(type) /* Assigning operands */
        {
            case register_ptr:
                lex->line.inst.operands[lex->line.inst.operand_cnt].optype = register_ptr;
                /* Recieving register number */
                lex->line.inst.operands[lex->line.inst.operand_cnt].operand.register_ptr = (int)strtol(strchr(sep->words_ptrs[i], 'r') + 1, &temp, 10);
                break;
            case register_direct:
                lex->line.inst.operands[lex->line.inst.operand_cnt].optype = register_direct;
                /* Recieving register number */
                lex->line.inst.operands[lex->line.inst.operand_cnt].operand.register_direct = (int)strtol(strchr(sep->words_ptrs[i], 'r') + 1, &temp, 10);
                break;
            case immediate:
                lex->line.inst.operands[lex->line.inst.operand_cnt].optype = immediate;
                /* Recieving operand integer */
                lex->line.inst.operands[lex->line.inst.operand_cnt].operand.immediate = (int)strtol(sep->words_ptrs[i] + 1, &temp, 10);
                break;
            case direct_label:
                lex->line.inst.operands[lex->line.inst.operand_cnt].optype = direct_label;
                /* Recieving label name */
                strcpy(lex->line.inst.operands[lex->line.inst.operand_cnt].operand.direct_label, sep->words_ptrs[i]);
                break;
            default:
            break;
        }
        lex->line.inst.operand_cnt++;
        i += 2; /* Skipping comma between operands */
    }
}


void dir_case(Line_lexer lex, Line_seperated sep, struct macro_sum macros, char * line)
{
    int i, in_string;
    enum type_inst type_inst;
    enum op_type type;
    char *temp;

    in_string = 0;
    i = 0;

    if (!check_label_and_errors(lex, sep, macros, DIR, &i)) /* errors have occured */
        return;

    type_inst = check_instruction(sep->words_ptrs[i]);
    lex->line.dir.type = type_inst;

    if (type_inst != string_inst) 
    {
        if (!check_line_comma_compatible(lex, sep)) /* Checking for comma overflow */
        {
            strcpy(lex->error_message, "Comma overflow");
            return;
        }
    }

    if (!check_operand_amount_compatible_dir(lex, sep, type_inst)) /* Operand amount is not compatible with the inst */ 
    {
        sprintf(lex->error_message, "Operand amount is not compatible with instruction %s", instructions[type_inst]);
        return;
    } 
    i++;
    lex->line.dir.dir_type.string[0] = '\0'; /* Setting string to empty */

    while (i < sep->ptr_cntr) 
    {
        type = operand_type_analyzer(sep->words_ptrs[i], macros, DIR, in_string); /* Recieving operand type */

        /* If it's an integer that's out of range and the directive is .data, the operand is invalid */
        if (type == out_of_range && check_operand_type_compatible_dir(type_inst, immediate)) 
        {
            sprintf(lex->error_message, "Number %s is out of range", sep->words_ptrs[i]);
            return;
        }

        if (type == invalid || !check_operand_type_compatible_dir(type_inst, type)) /* Operand is invalid or not comaptible with directive instruction */
        {
            sprintf(lex->error_message, "Operand type doesn't match instruction %s", instructions[type_inst]);
            return;
        }

        switch(type) /* Assigning operands */
        {
            case string:
                string_case(lex, line); /* Entering the special string case */
                return;

            case direct_label:
                strcpy(lex->line.dir.dir_type.string, sep->words_ptrs[i]); /* Recieving label name */
                break;
            case immediate:
                /* Recieving operand integer */
                lex->line.dir.dir_type.data.data_arr[lex->line.dir.dir_type.data.data_amount++] = (int)strtol(sep->words_ptrs[i], &temp, 10);
                break;
            default:
            break;
        }

        if (type_inst != string_inst) 
        {
            i += 2; /* Skipping comma between operands */
        }
        else 
        {
            i++;
        }
        
    }
}

enum op_type operand_type_analyzer(char *op, struct macro_sum macros, char mode, int in_string) 
{
    int index, state;
    char * label;

    if ((index = check_register(op)) >= 0) /* Checking whether the operand is a valid register */
    {
        if (check_register_ptr(op, index)) /* Checking if it's a register pointer */
        {
            return register_ptr;
        }
        return register_direct;
    }

    if (((state = is_number(op, mode)) != INT_MIN)) /* Checking if the number is valid */
    {
        if (state == INT_MAX) /* Numebr is out of range */
        {
            return out_of_range;
        }
        return immediate; /* Number is valid */
    }

    if (mode == DIR) /* Checking whether it's a directive instruction */
    {
        if (in_string || (!in_string && *op == '\"')) /* Checking if the string beginning is valid */
        {
            return string;
        }

        else if (!in_string && check_string(op)) /* Checking if it's a valid string with one word only */
        {
            return string;
        }
    }

    if ((label = is_valid_label(op, macros, LABEL_OP)) != NULL) /* Checking if it's a valid label */
    {
        free(label);
        if (in_string) 
        {
            return invalid;
        }
        return direct_label; /* Returning label only if it's not a string directive instruction */
    }

    return invalid; /* An invalid operand */
}


int check_how_many_operands(Line_seperated sep) 
{
    int i, j;

    for (i = 0, j = 0; i < sep->ptr_cntr; i++) /* Traversing seperator */
    {
        if (strcmp(sep->words_ptrs[i], ",") != 0) /* If the token is not ',' , it's an operand */
        {
            j++;
        }
    }

    return j;
}

int check_operand_type_compatible_inst(int opcode_index, enum op_type type, int operand_order) 
{
    if (operand_order == ORIGINAL) 
    {
        return ((opcodes[opcode_index].op_types_allowed_original) >> type) & 1; /* Checking if the binary digit of original operand is on */
    }

    return ((opcodes[opcode_index].op_types_allowed_destination) >> type) & 1; /* Checking if the binary digit of destination operand is on */
}

int check_operand_type_compatible_dir(enum type_inst type_inst, enum op_type type) 
{
    if (type_inst == string_inst) /* string */
    {
        return type == string;
    }

    else if (type_inst == external || type_inst == entry) /* entry, external */
    {
        return type == direct_label;
    }

    return type == immediate; /* data */
}

int check_operand_amount_compatible_inst(Line_lexer lex, Line_seperated sep, int opcode_i, int  * operand_amount) 
{
    int i;
    i = 1;

    if (is_label_initialized(lex)) /* Checking whether label is initialized to know from where to start counting opernads */
    {
        i = 2;
    }
    
    for (; i < sep->ptr_cntr; i++) 
    {
        if (strcmp(sep->words_ptrs[i], ",") != 0) /* Counting each token that is different than ',' */
        {
            ++*operand_amount;
        }
    }

    switch(*operand_amount) 
    {
        case 0: /* Checking whether the opearation recieves 0 opearnds */
            return opcodes[opcode_i].op_types_allowed_original == 0 && opcodes[opcode_i].op_types_allowed_destination == 0;
            break;
        case 1: /* Checking whether the operation recieves destination operand */
            return opcodes[opcode_i].op_types_allowed_original == 0 && opcodes[opcode_i].op_types_allowed_destination != 0;
            break;
        case 2: /* Checking whether the operation recieves both destination and original operands */
            return opcodes[opcode_i].op_types_allowed_original != 0 && opcodes[opcode_i].op_types_allowed_destination != 0;
            break;
        default:
            return 0;
    }
}

int check_operand_amount_compatible_dir(Line_lexer lex, Line_seperated sep, enum type_inst inst) 
{
    int i, operand_amount;
    operand_amount = 0;

    i = 1;

    if (is_label_initialized(lex)) /* Determining where to start counting operands */
    {
        i = 2;
    }
    
    for (; i < sep->ptr_cntr; i++) 
    {
        if (strcmp(sep->words_ptrs[i], ",") != 0) /* Counting each token that is different than ',' */
        {
            ++operand_amount;
        }
    }

    /* external and entry can only recieve one operand and each directive instruction recieves more than zero oparands */
    if (operand_amount == 0 || ((inst == external || inst == entry) && operand_amount != 1)) 
    {
        return 0;
    }

    return 1;
}


int is_label_initialized(Line_lexer lex) 
{
    return lex->label_name[0] != '\0'; /* Label name is not empty */
}


int check_line_comma_compatible(Line_lexer lex, Line_seperated sep) 
{
    int comma_counter, i;
    i = 1;

    if(is_label_initialized(lex)) /* Checking if label is initialized */
        i = 2;
    
    if (i < sep->ptr_cntr && strcmp(sep->words_ptrs[i], ",") == 0 ) /* Comma between operation\instruction and operands */
    {
        return 0;
    }

    i++;

    while (i < sep->ptr_cntr) 
    {
        comma_counter = 0;

        while (i < sep->ptr_cntr && strcmp(sep->words_ptrs[i], ",") == 0) /* Counting commas */
        {
            comma_counter++;
            i++;
        }
        i++;

        if (comma_counter != 1 || (i > sep->ptr_cntr && comma_counter > 0)) /* Comma overflow */
        {
            return 0;
        }
    }

    return 1;
}


int is_error_message_init(Line_lexer lex) 
{
    return lex->error_message[0] != '\0'; /* Error message is not empty  */
}


int check_reg(enum op_type type) 
{
    return type == register_ptr || type == register_direct; /* Checking whether the type is a direct or pointer of a register */
}

int string_case(Line_lexer lex, char *line) 
{
    char *str, *original, *temp;

    /* Find the first quotation mark */
    str = strchr(line, '\"');

    if (str == NULL) /* No opening quotation mark */
    {
        strcpy(lex->error_message, "Opening string quotation mark \" never appeared in string directive");
        return 0;
    }
    
    str++; 
    my_strdup(&original, str); /* Duplicate the string for manipulation */

    strtok(str, "\"");
    temp = strchr(original, '\"');

    if (temp == NULL) /* No closing quotation mark */
    {
        strcpy(lex->error_message, "Closing string quotation mark \" never appeared in string directive");
        free_handler(original);
        return 0;
    }

    temp++;
    SKIP_SPACES(temp);

    if (*temp != '\0') /* There were additional chars after string closing */
    {
        strcpy(lex->error_message, "Additional chars after string ended");
        free_handler(original);
        return 0;
    }

    if (str != NULL) /* String is valid */
    {
        strcpy(lex->line.dir.dir_type.string, str);
        free_handler(original);
        return 1;
    }

    strcpy(lex->error_message, "String is not valid");
    free_handler(original);
    return 0;
}
