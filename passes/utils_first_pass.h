#ifndef UTILS_FIRST_PASS_H
#define UTILS_FIRST_PASS_H

#include "../data_stractures/dynamic_arr.h"
#include "../data_stractures/linkedlist.h"
#include "../utils/tools.h"
#include <stdarg.h>

#define REALLOC_CONSTANT 10

/* Representation of the translation unit*/
struct translation_unit 
{
    Dynamic_arr data_Image; /* Dynamic array for the data image */
    Dynamic_arr code_Image; /* Dynamic array for the code image */
    /* IC and DC counters*/
    int IC;
    int DC;
    /* All symbol tables */
    Dynamic_arr symTable;
    Dynamic_arr extSymTable;
    Dynamic_arr entSymTable;
};

enum symbol_state /* Enum for the symbol state */
{
    external_sym,
    entry_sym,
    entryCode,
    entryData,
    sym_code,
    sym_data
};


/* Representation of a symbol */
struct symbol 
{
    char symbol_name[MAX_LABEL_LENGTH]; /* Symbol name */
    enum symbol_state symbol_state; /* Symbol state */

    union 
    {
        int def_address; /* Addresss of the symbol (it it's not external) */
        struct 
        {
            int * external_sym_address_instances; /* Array of ints for external symbol instances */
            int instances_cnt; /* instances counter */
        } instances;
    } type;
    
};

typedef struct translation_unit * Translation_unit; /* Pointer of translation unit */
typedef struct symbol * Symbol; /* Pointer of symbol */


/**
 * @brief this function creates a symbol object by its name, address and state.
 * 
 * @param symbol_name the symbol name.
 * @param address the symbol address (parameter is not treated if it's external).
 * @param symbol_state the symbol state.
 * @return the final symbol, or NULL if symbol createion was unsuccessful.
 */
Symbol create_symbol(char * symbol_name, int address, int symbol_state);
/**
 * @brief Symbol constructor for symbol tables (creates a shallow copy to avoid multiple table traversals)
 * 
 * @param sym symbol to be copied.
 * @return a shallow copy of the symbol.
 */
Symbol symbol_ctor(Symbol sym);
/**
 * @brief This function destroys a given symbol.
 * 
 * @param sym symbol to be destroyed.
 */
void symbol_dtor(Symbol sym); 
/**
 * @brief This function adds an instance (address) to extern symbol instances integer array.
 * 
 * @param sym the external symbol.
 * @param inst the address (instance).
 * @return The function returns 1 if insertion was successful and 0 otherwise.
 */
int add_instance_to_extern_symbol(Symbol sym, int inst);
/**
 * @brief This function creates a translation unit object.
 * 
 * @param sym_ctor the symbol constructor (needed for symbol tables).
 * @param sym_dtor the symbol destructor (needed for symbol tables).
 * @return The final translation unit object, or NULL if creation was unsuccessful.
 */
Translation_unit create_translation_unit(void *(*sym_ctor)(const void *), void (*sym_dtor)(void *));
/**
 * @brief This function destroyes a translation unit object. 
 * 
 * @param unit_ptr pointer to a pointer to the translation unit.
 */
void destroy_translation_unit(Translation_unit * unit_ptr);
/**
 * @brief This function conducts a comparison between two symbols by their names.
 * 
 * @param s1 The first symbol.
 * @param s2 The second symbol.
 * @return returns 1 if a match was found, 0 otherwise.
 */
int symbol_cmp(Symbol s1, Symbol s2);
/**
 * @brief This function destroys the symbol tables in the translation units.
 * 
 * @param unit a pointer to a pointer to the translation unit.
 */
void dynamic_arr_destroyer_symbols(Translation_unit * unit);
/**
 * @brief This function destroyes a symbol and turn it's pointer to null.
 * 
 * @param sym a pointer to a pointer to a symbol.
 */
void temp_symbol_dtor(Symbol * sym);

#endif