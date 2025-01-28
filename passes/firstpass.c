#include "../passes/firstpass.h"
#include "../passes/utils_first_pass.h"
#include "../lexer/lexer.h"
#include "../utils/errors.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>


void first_pass(char *file_name, struct macro_sum * macros) 
{
    FILE * fp;
    Translation_unit unit;
    Error_location location;
    Line_lexer lex;
    Line_seperated sep;
    char * final_file_name, buffer[MAX_LINE_LENGTH], buf_cpy[MAX_LINE_LENGTH], *temp;
    int error_found = 0; /* No error has been found */

    /* Allocating memory to structs */
    lex = (Line_lexer)(handle_malloc(sizeof(struct line_lexer)));
    sep = (Line_seperated)(handle_malloc(sizeof(struct line_seperated)));
    location = (Error_location)(handle_malloc(sizeof(struct error_location)));
    unit = create_translation_unit((void *(*)(const void *))symbol_ctor, (void (*)(void *))symbol_dtor);
    final_file_name = file_name_generator(file_name, ".am");
    fp = fopen_handler(final_file_name, "r");
    memset(location, 0, sizeof(struct error_location));
    memset(sep, 0, sizeof(struct line_seperated));
    my_strdup(&location->file_name, final_file_name);

    location->line = 1; /* Line counter begins from 1*/

    if (check_if_null(7, location, unit, final_file_name, fp, lex, sep, location->file_name, macros)) 
    {
        /* Cleaning up memory if any allocation has been failed*/
        CLEAN_UP_FIRST_PASS(final_file_name, unit, location, sep, lex, fp, macros);
        return;
    }
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) /* Traversing file*/
    {
        strcpy(buf_cpy, buffer);
        if ((temp = strpbrk(buf_cpy, LINE_CLOSERS)) != NULL) 
        {
            *temp = '\0';
        }
        
        memset(lex, 0, sizeof(struct line_lexer));

        if (!line_seperator(sep, buf_cpy) || !main_lexer(lex, sep, *macros, buffer)) /* Line seperation or line lexer found problem */
        {
            error_found = 1;
            PRINT_ERROR_MESSAGE(lex, location);
            free_line_seperator(sep);
            location->line++;
            continue;
        }

        if (lex->line_type == inst) 
        {
            if (!inst_convertion(lex, unit)) /* Going to inst line convertion */
            {
                /* Error has been found*/
                error_found = 1;
                PRINT_ERROR_MESSAGE(lex, location);
            }
        }

        else if (lex->line_type == dir) /* Going to dir line convertion */ 
        {
            /* Error has been found */
            if (!dir_convertion(lex, unit)) 
            {
                error_found = 1;
                PRINT_ERROR_MESSAGE(lex, location);
            }
        }

        free_line_seperator(sep);
        location->line++;
    }

    if (unit->IC + unit->DC + 100 > MAX_MEMORY_SIZE) /* Memory overflow*/
    {
        printf("Error: Memory overflow\n");
        CLEAN_UP_FIRST_PASS(final_file_name, unit, location, sep, lex, fp, macros);
        free(sep);
        destroy_translation_unit(&(unit));
        return;
    }

    if (!error_found) /* No errors in first pass, moving to second pass*/
    {
        change_label_address(unit);
        printf("Entering second pass\n");
        if (!second_pass(file_name, fp, unit, lex, sep, macros, location)) /* Second pass failed */
        {   
            printf("Second pass failed\n");
        }
        CLEAN_UP_FIRST_PASS(final_file_name, unit, location, sep, lex, fp, macros);
        free(sep);
        destroy_translation_unit(&(unit));
        return;
    }

    /* First pass failed, there were errors */
    printf("First pass failed\n");
    /* Cleaning up all memory*/
    CLEAN_UP_FIRST_PASS(final_file_name, unit, location, sep, lex, fp, macros);
    destroy_translation_unit(&(unit));
    free(sep);
    return;
}


int inst_convertion(Line_lexer lex, Translation_unit unit) 
{
    int i, is_sym, def;
    short next_word;
    Symbol sym;
    void * status;

    is_sym = is_label_initialized(lex); /* Checking if label has been defined */

    if (is_sym) 
    {
        sym = create_symbol(lex->label_name, unit->IC, sym_code); /* Creating label */
        def = check_for_definitions(unit, sym, lex, entryCode, unit->IC); /* Checking for definition states*/

        if (is_sym && def == MULTIPLE_DEF) /* There was a multiple definition */
        {
            symbol_dtor(sym);
            return 0;
        }   
    }

    next_word = 0;

     /* Handling First Word */
    next_word |= (lex->line.inst.opcode << OPCODE_MOVE);
    next_word |= (1 << A);

    switch(lex->line.inst.operand_cnt) /* Checking for amount of operands and building first word */ 
    {
        case 1:
        next_word |= (1 << (ADDRESS_MOVE_DEST + lex->line.inst.operands[0].optype));
        break;

        case 2:
        next_word |= (1 << (ADDRESS_MOVE_ORIG + lex->line.inst.operands[0].optype));
        next_word |= (1 << (ADDRESS_MOVE_DEST + lex->line.inst.operands[1].optype));

        default:
        break;
    }

    INSERT_ELEMENT(unit->code_Image, status, next_word, unit->IC); /* Inserting word to code image */

    if (check_reg(lex->line.inst.operands[0].optype) && check_reg(lex->line.inst.operands[1].optype)) /* In case of both registers, they share the same word*/
    {
        next_word |= (lex->line.inst.operands[0].operand.register_ptr << REGISTER_MOVE_ORIG);
        next_word |= (lex->line.inst.operands[1].operand.register_ptr << REGISTER_MOVE_DEST);
        next_word |= (1 << A);
        INSERT_ELEMENT(unit->code_Image, status, next_word, unit->IC); /* Inserting word to code image */
        if (def == NOT_FOUND) /* First definition of label */
        {
            INSERT_SYMBOL(unit->symTable, status, sym, unit->symTable->item_cnt, NEXT_NULL); /* Inserting symbol to symbol table */
        }

        else if (def == FOUND_ENTRY) /* Destroying symbol if it has been found entry (Symbol is altered in check_for_definitions) */
        {
            symbol_dtor(sym);    
        }
        return 1;
    }

    for(i = 0; i < OPERAND_MAX; i++) /* Traversing operands */
    {
        if (lex->line.inst.operands[i].optype == direct_label) /* Handling direct labels in second pass */ 
        {
            unit->IC++;
            continue;
        }
            
        else if (check_reg(lex->line.inst.operands[i].optype)) 
        {
            if ((i == 0 && lex->line.inst.operand_cnt == 1) || i != 0) /* Only dest reg operand*/ 
            {
                next_word |= (lex->line.inst.operands[i].operand.register_direct << REGISTER_MOVE_DEST);
                next_word |= (1 << A);
                INSERT_ELEMENT(unit->code_Image, status, next_word, unit->IC); /* Inserting word to code image */
            }   

            else if (i == 0 && lex->line.inst.operand_cnt != 1) /* Original operand */
            {
                next_word |= (lex->line.inst.operands[i].operand.register_direct << REGISTER_MOVE_ORIG);
                next_word |= (1 << A);
                INSERT_ELEMENT(unit->code_Image, status, next_word, unit->IC); /* Inserting word to code image */
            }
        }

        else 
        {
            next_word |= (lex->line.inst.operands[i].operand.immediate << IMMEDIATE_MOVE); /* Immediate operand */

            if (next_word != 0) 
            {
                next_word |= (1 << A);
                INSERT_ELEMENT(unit->code_Image, status, next_word, unit->IC); /* Inserting word to code image */
            }
        }
    }

    if (def == NOT_FOUND) /* First definition of label */
    {
        INSERT_SYMBOL(unit->symTable, status, sym, unit->symTable->item_cnt, NEXT_NULL); /* Inserting symbol to symbol table */
    }

    else if (def == FOUND_ENTRY) /* Destroying symbol if it has been found entry (Symbol is altered in check_for_definitions) */
    {
        symbol_dtor(sym);    
    }
    return 1;
}

int dir_convertion(Line_lexer lex, Translation_unit unit) 
{
    int i, is_sym, def;
    short next_word;
    void * status;
    Symbol sym, temp_sym, sym_check;
    sym = NULL;

    is_sym = is_label_initialized(lex);

    if (is_sym) 
    {
        sym = create_symbol(lex->label_name, unit->DC, sym_data);
        def = check_for_definitions(unit, sym, lex, entryData, unit->DC);

        if (is_sym && !def) /* There was a multiple definition */
        {
            return 0;
        }   
    }

    next_word = 0;

    if ((lex->line.dir.type == external || lex->line.dir.type == entry) && is_sym) /* Label with no meaning in the beginning of extern/entry instruction */
    {
        printf("Warning: %s is in the beginning of extern/entry instruction\n", lex->label_name);
        symbol_dtor(sym);
        return 1;
    }

    if (lex->line.dir.type == data) /* Data instruction. Creating a word for each short int given as operand */
    {
        for (i = 0; i < lex->line.dir.dir_type.data.data_amount; i++) 
        {
            next_word = (short)lex->line.dir.dir_type.data.data_arr[i];
            INSERT_ELEMENT(unit->data_Image, status, next_word, unit->DC); /* Inserting word to code image */
        }
    }

    else if (lex->line.dir.type == string_inst) /* String instruction. Creating a word for each char including null terminator */
    {
        for (i = 0; lex->line.dir.dir_type.string[i] != '\0'; i++) 
        {
            next_word = lex->line.dir.dir_type.string[i];
            INSERT_ELEMENT(unit->data_Image, status, next_word, unit->DC);
        }

        next_word = '\0';
        INSERT_ELEMENT(unit->data_Image, status, next_word, unit->DC);
    }

    else if (lex->line.dir.type == external) 
    {
        temp_sym = create_symbol(lex->line.dir.dir_type.string, 1, external_sym);
        sym_check = search_dynamic_array(unit->symTable, temp_sym, (int (*)(void *, void *))symbol_cmp); /* Searching if symbol already exists */

        if (sym_check == NULL) /* Checking for multiple definitions */  
        {
            /* Adding both to symbol table and extern symbol table*/
            INSERT_SYMBOL(unit->symTable, status, temp_sym, unit->symTable->item_cnt, NEXT_NULL);
            INSERT_SYMBOL(unit->extSymTable, status, temp_sym, unit->extSymTable->item_cnt, NEXT_NULL);
        }
        else if (sym_check != NULL && sym_check->symbol_state != external_sym) /* Checking if symbol was declared as external and anything else */
        {
            switch(sym_check->symbol_state) 
            {
                case entry_sym:
                case entryCode:
                case entryData:
                    sprintf(lex->error_message, "label %s defined both as external and entry", temp_sym->symbol_name);
                break;
                case sym_code:
                    sprintf(lex->error_message, "label %s defined both as external and code", temp_sym->symbol_name);
                break;
                case sym_data:
                    sprintf(lex->error_message, "label %s defined both as external and data", temp_sym->symbol_name);
                break;
                default:
                break;
            }   
            symbol_dtor(temp_sym);
            symbol_dtor(sym);
            return 0;
        }
    }

    else if (lex->line.dir.type == entry)
    {
        temp_sym = create_symbol(lex->line.dir.dir_type.string, 0, entry_sym);

        /* Label was defined before declared as entry */
        if ((sym_check = search_dynamic_array(unit->symTable, temp_sym, (int (*)(void *, void *))symbol_cmp)) != NULL) 
        {
            if (sym_check->symbol_state != entry_sym && sym_check->symbol_state != entryCode && sym_check->symbol_state != entryData) 
            {
                sym_check->symbol_state = sym_check->symbol_state == sym_code ? entryCode : entryData;
                INSERT_SYMBOL(unit->entSymTable, status, sym_check, unit->entSymTable->item_cnt, NEXT_NULL); /* Adding symbol to entry symbol table */
            }
            symbol_dtor(temp_sym);
        }

        else if (search_dynamic_array(unit->extSymTable, temp_sym, (int (*)(void *, void *))symbol_cmp) == NULL) /* Checking for multiple definitions */ 
        {
            /* Adding both to symbol table and extern symbol table */
            INSERT_SYMBOL(unit->symTable, status, temp_sym, unit->symTable->item_cnt, NEXT_NULL);
            INSERT_SYMBOL(unit->entSymTable, status, temp_sym, unit->entSymTable->item_cnt, NEXT_NULL);
        }

        /* Label is defined both as entry and external */
        else 
        {
            sprintf(lex->error_message, "label %s defined both as external and entry", temp_sym->symbol_name);
            symbol_dtor(temp_sym);
            symbol_dtor(sym);
            return 0;
        }
    }

    if (def == NOT_FOUND) /* First definition of label */
    {
        INSERT_SYMBOL(unit->symTable, status, sym, unit->symTable->item_cnt, NEXT_NULL);
    }

    else if (def == FOUND_ENTRY) /* Destroying symbol if it has been found entry (Symbol is altered in check_for_definitions) */
    {
        symbol_dtor(sym);    
    }
    return 1;
} 


int check_for_definitions(Translation_unit unit, Symbol sym, Line_lexer lex, enum symbol_state state, int address) 
{
    Symbol sym_check;

    if ((sym_check = search_dynamic_array(unit->symTable, sym, (int (*)(void *, void *))symbol_cmp)) != NULL) /* Checking if label exists */
    {
        if (sym_check->symbol_state != entry_sym) /* Multiple definitions of label */
        {
            sprintf(lex->error_message, "Multiple defintions of %s: Latest definition ", sym->symbol_name);
            symbol_dtor(sym);
            return MULTIPLE_DEF;            
        }

        /* Symbol was found and defined entry. Now we know it's a data type of entry*/
        sym_check->symbol_state = state;
        sym_check->type.def_address = address;
        return FOUND_ENTRY;
    }

    return NOT_FOUND;
}

void change_label_address(Translation_unit unit) 
{
    int i, state;

    for (i = 0; i < unit->symTable->item_cnt; i++) /* Traversing symbol table */
    {
        state = ((Symbol)unit->symTable->ptr_arr[i])->symbol_state;
        if (state == entryCode || state == sym_code) 
        {
            ((Symbol)unit->symTable->ptr_arr[i])->type.def_address += 100; /* If it's a code, adding 100 */
        }

        else if (state == entryData || state == sym_data) 
        {
            ((Symbol)unit->symTable->ptr_arr[i])->type.def_address += (unit->IC + 100); /* If it's a data, adding IC + 100 */
        }
    }
}

int second_pass(char * file_name, FILE * fp ,Translation_unit unit, Line_lexer lex, Line_seperated sep, struct macro_sum * macros, Error_location location) 
{
    char buffer[MAX_LINE_LENGTH], buf_cpy[MAX_LINE_LENGTH], *temp;
    int error_found;
    error_found = 0;

    unit->IC = unit->DC = 0; /* Reseting IC and DC */
    fseek(fp, 0, SEEK_SET); /* Returning to beginning of file */
    location->line = 1;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) 
    {
        strcpy(buf_cpy, buffer);
        if ((temp = strchr(buf_cpy, '\n')) != NULL) 
        {
            *temp = '\0';
        }
        
        memset(lex, 0, sizeof(struct line_lexer));
        memset(sep, 0, sizeof(struct line_seperated));

        /* Building a lexer stracture for line (Can't be failing, only in first pass)*/
        line_seperator(sep, buf_cpy);
        main_lexer(lex, sep, *macros, buffer);

        if (!second_pass_main(unit, lex)) /* second_pass_main failed */
        {
            if (is_error_message_init(lex)) 
            {
                error_found = 1;
                PRINT_ERROR_MESSAGE(lex, location);
            }
        }

        free_line_seperator(sep);
        location->line++;
    }

    if(!error_found) 
    {
        create_final_files(unit, file_name); /* Creating final files only if both passes were with no errors */ 
        return 1;
    }
    return 0;
}   

int second_pass_main(Translation_unit unit ,Line_lexer lex) 
{
    int i;
    Symbol sym, temp_sym;
    void * status;
    short word;

    if (lex->line_type == inst) 
    {
        unit->IC++; /* For first word of instruction */

        /* If the function has two operands and their both a type of register, they take one word */
        if (check_reg(lex->line.inst.operands[0].optype) && check_reg(lex->line.inst.operands[1].optype)) 
        {
            unit->IC++;
            return 1;
        }

        for (i = 0; i < lex->line.inst.operand_cnt; i++) 
        {
            if (lex->line.inst.operands[i].optype == direct_label) 
            {
                temp_sym = create_symbol(lex->line.inst.operands[i].operand.direct_label, 0, 0);
                sym = search_dynamic_array(unit->symTable, temp_sym, (int (*) (void *, void *))symbol_cmp);

                if (sym != NULL) 
                {
                    if (sym->symbol_state == external_sym) 
                    {
                        add_instance_to_extern_symbol(sym, unit->IC + 100); /* Adding address as an instance to extern symbol array of instances */
                        word = 1;
                    }
                    
                    else 
                    {
                        /* Creating word for label */
                        word = sym->type.def_address << DIRECT_LABEL_MOVE; 
                        word |= (1 << R);
                    }
        
                    INSERT_ELEMENT(unit->code_Image, status, word, unit->IC); /* Inserting word to code image */
                }

                if (sym == NULL) /* Symbol serves as an operand but is never defined */
                {
                    sprintf(lex->error_message, "label %s serves as an operand, but never defined", lex->line.inst.operands[i].operand.direct_label);
                    temp_symbol_dtor(&temp_sym);
                    return 0;
                }

                temp_symbol_dtor(&temp_sym);
            }

            else 
            {
                unit->IC++;
            }
        }
    }

    else 
    {
        if (lex->line.dir.type == string_inst) 
        {
            unit->DC += (strlen(lex->line.dir.dir_type.string) + 1); /* DC grows by the number of chars in the string with null terminator*/
        }

        else if (lex->line.dir.type == data) 
        {
            unit->DC += lex->line.dir.dir_type.data.data_amount; /* DC grows by the amount of numbers in a data directive */
        }
    }

    return 1;
}

int create_final_files(Translation_unit unit, char * file_name) 
{
    FILE *fp_ext_file, *fp_ent_file, *fp_object_file;
    char *file_name_ext, *file_name_ent, *file_name_obj;
    int i, j;
    char octal[OCTAL_LENGTH];

    fp_ent_file = fp_ext_file = fp_object_file = NULL; /* Reseting all file pointers to null */

    /* Generating final file names*/
    file_name_ext = file_name_generator(file_name, ".ext");
    file_name_ent = file_name_generator(file_name, ".ent");
    file_name_obj = file_name_generator(file_name, ".ob");

    /* Checking if generating file names was succesfull*/
    if (check_if_null(3, file_name_ent, file_name_ext, file_name_obj)) 
    {
        printf("Files were not created succesfully...\n");
        print_internal_error(ERROR_CODE_4);
        return 0;
    }

    if (unit->entSymTable->item_cnt != 0) /* Checking if there were entry symbols */
        fp_ent_file = fopen_handler(file_name_ent, "w");
    if (unit->extSymTable->item_cnt != 0)  /* Checking if there were extern symbols */
        fp_ext_file = fopen_handler(file_name_ext, "w");
    if (unit->IC != 0 || unit->DC != 0) /* Checking if there were lines in file*/
        fp_object_file = fopen_handler(file_name_obj, "w");


    if (fp_object_file != NULL) 
    {
        fprintf(fp_object_file, "   %d  %d\t\n", unit->IC, unit->DC); /* Printing IC and DC in correct format */

        for (i = 0; i < unit->IC; i++) 
        {   
            octal[0] = '\0';
            short_to_octal(unit->code_Image->ptr_arr[i], octal); /* Translating from short to octal */
            fprintf(fp_object_file, "%04d   %s\n", (i + 100), octal); /* Printing words to object file from code image */
        }

        for (i = 0; i < unit->DC; i++) 
        {
            octal[0] = '\0';
            short_to_octal(unit->data_Image->ptr_arr[i], octal); /* Translating from short to octal */
            fprintf(fp_object_file, "%04d   %s\n", (unit->IC + 100 + i), octal); /* Printing words to object file from data image */
        }

        printf("%s created succesfully\n", file_name_obj);
    }

    if (fp_ent_file != NULL) 
    {
        for (i = 0; i < unit->entSymTable->item_cnt; i++) /* Printing all entry symbols to entry file*/
        {
            fprintf(fp_ent_file ,"%s %d\n", ((Symbol)(unit->entSymTable->ptr_arr[i]))->symbol_name, ((Symbol)(unit->entSymTable->ptr_arr[i]))->type.def_address);
        }

        printf("%s created succesfully\n", file_name_ent);
    } 

    if (fp_ext_file != NULL) 
    {
        for (i = 0; i < unit->extSymTable->item_cnt; i++) /* Printing all extern symbols to extern file */
        {
            for (j = 0; j < ((Symbol)(unit->extSymTable->ptr_arr[i]))->type.instances.instances_cnt; j++) 
            {
                fprintf(fp_ext_file, "%s %04d\n", ((Symbol)(unit->extSymTable->ptr_arr[i]))->symbol_name ,((Symbol)(unit->extSymTable->ptr_arr[i]))->type.instances.external_sym_address_instances[j]);
            }   
        }

        printf("%s created succesfully\n", file_name_ext);
    }

    /* Cleaning up memory for file names and closing all files */
    CLEAN_UP_FILE_CREATOR(fp_ent_file, fp_object_file, fp_ext_file, file_name_ext, file_name_obj, file_name_ent);
    return 1;
}