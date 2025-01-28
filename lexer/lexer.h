#ifndef LEXER_H
#define LEXER_H

#include "../preasm/macro_handler.h"
#define WHITE_CHARS_COMMA " \t\v,"
#define MAX_OPERANDS_INST 4 /* Max operands that are possible in an instruction line */
#define ERROR_MESSAGE_SIZE 256 /* String constant for error message */
#define OPERANDS 2
#define ORIGINAL 1
#define DESTINATION 2


/* Macro for adding word to line_seperated struct (used in line_seperator)*/
#define ADD_WORD_TO_SEP(line, new_seperator)                              \
        (new_seperator)->words_ptrs[(new_seperator)->ptr_cntr] =                \
            handle_malloc(sizeof(char) * strlen(line) + 1);                     \
        if ((new_seperator)->words_ptrs[(new_seperator)->ptr_cntr] == NULL)     \
        {                                                                       \
            free_line_seperator(new_seperator);   \
            free(new_seperator);                              \
            return 0;                                                           \
        }                                                                       \
        strcpy((new_seperator)->words_ptrs[(new_seperator)->ptr_cntr], line);   \
        (new_seperator)->ptr_cntr++;                                            \


enum op_type /* All possible operand types */
{
    immediate,
    direct_label, 
    register_ptr,
    register_direct,
    string,
    out_of_range,
    invalid  
};


enum type_inst /* All possible instruction types for direct lines */
{
    data, 
    string_inst, 
    entry,
    external
};


/**
 * @brief This is a struct that summarises an entire line.
 * It has a field for an error message if there was a problem in the line,
 * and a filed for the label name if one has been defined in the beggining of the line.
 * It has a field for the line type (whether it's instruction, directive or an empty line), and 
 * union that summarizes the rest of the line based on its type (meaning it will assign an opcode and the necessary operands based
 * on their types, or if it's a directive, all the integers of the .data instruction or the string in the .string instruction).
 */
struct line_lexer 
{
    char error_message[ERROR_MESSAGE_SIZE];
    char label_name[MAX_LABEL_LENGTH];
    
    enum 
    {
        inst,
        dir, 
        empty
    } line_type;

    union 
    {
        struct 
        {
            enum 
            {
                mov, cmp, add, sub, lea, clr, Not, inc, dec, jmp, bne, red, prn, jsr, rts, stop
            } opcode;

            struct 
            {
                enum op_type optype;

                union 
                {
                    int immediate;
                    char direct_label[MAX_LABEL_LENGTH];
                    int register_ptr;
                    int register_direct;
                } operand;
            } operands[OPERANDS];

            int operand_cnt;
        } inst; 

        struct 
        {
            enum type_inst type;

            union 
            {
                char string[MAX_LINE_LENGTH];
                
                struct 
                {
                    int data_arr[MAX_LINE_LENGTH];
                    int data_amount;
                } data;
            } dir_type;
        } dir;
    } line;
};


/* Struct that representing an array of strings, the summarizes the lines of the file */
struct line_seperated 
{
    char * words_ptrs[MAX_LINE_LENGTH];
    int ptr_cntr;
};

typedef struct line_seperated * Line_seperated; /* A pointer to line_seperated */
typedef struct line_lexer * Line_lexer; /* A pointer to line_lexer */

/**
 * @brief This function takes a line and breaks it to tokens seperated by spaces (white chars).
 * It manipulates new_seperator by adding all the tokens to its array of strings, where each token is a string literal or a comma.
 * 
 * @param new_seperator struct of the array of tokens.
 * @param line line of the file.
 * @return 1 whether seperation succesfull or 0 otherwise.
 */
int line_seperator(Line_seperated new_seperator, char *line);
/**
 * @brief This function takes a pointer to line_seperated and frees the memory that was allocated to it
 * 
 * @param sep pointer to line_seperated
 */
void free_line_seperator(Line_seperated sep);
/**
 * @brief This function takes the array of tokens (the seperated line) and it manipulates the line_lexer to create a full summarization of the line.
 * 
 * @param lex a pointer to line_lexer.
 * @param sep a pointer to line_seperated.
 * @param macros array of macro names that's being created in the preassembler stage.
 * @param line line from the file.
 * @return 1 if lexering succesfull or 0 otherwise.
 */
int main_lexer(Line_lexer lex, Line_seperated sep, struct macro_sum macros, char * line);
/**
 * @brief This function handles the case of a instruction line and alters line lexer accordingly.
 * 
 * @param lex a pointer to line_lexer.
 * @param sep a pointer to line_seperated.
 * @param macros array of macro names that's being created in the preassembler stage.
 */
void inst_case(Line_lexer lex, Line_seperated sep, struct macro_sum macros);
/**
 * @brief This function handles the case of a directive line and alters line lexer accordingly.
 * 
 * @param lex a pointer to line_lexer.
 * @param sep a pointer to line_seperated.
 * @param macros array of macro names that's being created in the preassembler stage.
 * @param line line from the file
 */
void dir_case(Line_lexer lex, Line_seperated sep, struct macro_sum macros, char * line);
/**
 * @brief This function checks the amount of operands in a line (both for directive and instruction)
 * 
 * @param sep a pointer to line_seperated.
 * @return the amount of operands in the line.
 */
int check_how_many_operands(Line_seperated sep);
/**
 * @brief This function takes an operand (string) and returns its type.
 * 
 * @param op a string that represents the operand.
 * @param macros array of macro names thatn's being created in the preassembler stage.
 * @param mode char that differentiates between instruction and directive.
 * @param in_string a flag for whether i'm currently in a string.
 * @return returns the operand type. 
 */
enum op_type operand_type_analyzer(char *op, struct macro_sum macros, char mode, int in_string);
/**
 * @brief This function checks whether an operand type is compatible with an operation.
 * 
 * @param opcode_index index that represents the operation.
 * @param type type of operand.
 * @param operand_order indicator of whether i'm in a destination operand or original.
 * @return returns 1 if the operand is compatible with the operation or 0 otherwise.
 */
int check_operand_type_compatible_inst(int opcode_index, enum op_type type, int operand_order);
/**
 * @brief This function checks whether an operand type is compatible with a directive instruciton.
 * 
 * @param type_inst the instruction type. 
 * @param type the type of operand.
 * @return return 1 if the operand is compatible with the diretive instruction or 0 otherwise.
 */
int check_operand_type_compatible_dir(enum type_inst type_inst, enum op_type type);
/**
 * @brief Checks whether the label is initialized or not.
 * 
 * @param lex a pointer to line_lexer.
 * @return returns 1 if the label is initialized or 0 otherwise.
 */
int is_label_initialized(Line_lexer lex);
/**
 * @brief The function checks if the amount of commas in the line is compatible and doesn't create an overflow/underflow.
 * 
 * @param lex a pointer to line_lexer.
 * @param sep a pointer to line_seperator.
 * @return return1 if the amount of commas is valid or 0 otherwise.
 */
int check_line_comma_compatible(Line_lexer lex, Line_seperated sep);
/**
 * @brief This function checks if the amount of operands is compatible with the operation.
 * 
 * @param lex a pointer to line_lexer.
 * @param sep a pointer to line_seperator.
 * @param opcode_i index that represents the opcode.
 * @param operand_amount The resulting operand amount
 * @return the function returns 1 if the amount is compatible or 0 otherwise.
 */
int check_operand_amount_compatible_inst(Line_lexer lex, Line_seperated sep, int opcode_i, int  * operand_amount);
/**
 * @brief This function checks if the amount of operands is compatible with the directive instruction.
 * 
 * @param lex a pointer to line_lexer.
 * @param sep a pointer to line_seperator.
 * @param inst The directive instruction.
 * @return returns 1 if the amount is compatible or 0 otherwise.
 */
int check_operand_amount_compatible_dir(Line_lexer lex, Line_seperated sep, enum type_inst inst);
/**
 * @brief Checks whether the error message is initialized. 
 * 
 * @param lex a pointer to line_lexer
 * @return returns 1 if the error message is indeed initialized or 0 otherwise.
 */
int is_error_message_init(Line_lexer lex);
/**
 * @brief This function checks various errors with the line (label issues).
 * 
 * @param lex a pointer the line_lexer.
 * @param sep a pointer to line_seperated.
 * @param macros array of macro names thatn's being created in the preassembler stage.
 * @param mode represents whether it's an instruction line or a directive.
 * @param i represents the index of operand searching for other functions.
 * @return 1 if errors were not found or 0 otherwise. 
 */
int check_label_and_errors(Line_lexer lex, Line_seperated sep, struct macro_sum macros, char mode, int * i);
/**
 * @brief This function returns if the type of the operand is register.
 * 
 * @param type type of the operand
 * @return 1 if it's indeed an integer or 0 otherwise.
 */
int check_reg(enum op_type type);
/**
 * @brief This function handles the special case of a string directive instruction.
 * 
 * @param lex a pointer to line_lexer.
 * @param line line from the file.
 * @return returns 1 if the string was valid, or 0 otherwise.
 */
int string_case(Line_lexer lex, char * line);

#endif