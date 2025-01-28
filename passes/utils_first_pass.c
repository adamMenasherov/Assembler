#include "../passes/utils_first_pass.h"
#include "../data_stractures/linkedlist.h"
#include <string.h>
#include <stdlib.h>

Translation_unit create_translation_unit(void *(*sym_ctor)(const void *), void (*sym_dtor)(void *)) 
{
    Translation_unit unit;
    unit = (Translation_unit)(handle_malloc(sizeof(struct translation_unit))); /* Allocating memory for Translation unit */

    if (unit == NULL) 
    {
        return NULL;
    }

    memset(unit, 0, sizeof(struct translation_unit)); /* Setting all bytes to 0 */

    /* Creating all dynamic arrays in unit */
    unit->data_Image = create_arr((void * (*)(const void *))short_ctor, (void (*)(void *))short_dtor);
    unit->code_Image = create_arr((void * (*)(const void *))short_ctor, (void (*)(void *))short_dtor);
    unit->symTable = create_arr(sym_ctor, sym_dtor);
    unit->extSymTable = create_arr(sym_ctor, sym_dtor);
    unit->entSymTable = create_arr(sym_ctor, sym_dtor);

    if (check_if_null(5, unit->symTable, unit->extSymTable, unit->entSymTable, unit->data_Image, unit->code_Image)) /* Destroying if anything was found null */
    {
        destroy_translation_unit(&unit);
        return NULL;
    }

    return unit;
}

void destroy_translation_unit(Translation_unit *unit_ptr) 
{
    /* Destroying all arrays */
    dynamic_arr_destroyer(&((*unit_ptr)->symTable));
    dynamic_arr_destroyer(&((*unit_ptr)->code_Image));
    dynamic_arr_destroyer(&((*unit_ptr)->data_Image));
    dynamic_arr_destroyer_symbols(unit_ptr);
    free(*unit_ptr);
    unit_ptr = NULL;
}

Symbol create_symbol(char * symbol_name, int address, int symbol_state) 
{
    Symbol new_symbol;
    new_symbol = (Symbol)(handle_malloc(sizeof(struct symbol))); /* Allocating memory for new symbol */

    if (new_symbol == NULL) /* Checking if allocation was successful */
    {
        return NULL;
    }

    /* Setting all bytes to 0 */
    memset(new_symbol, 0, sizeof(struct symbol));
    strcpy(new_symbol->symbol_name, symbol_name); /* Setting symbol name */

    if (symbol_state != external_sym) 
    {
        new_symbol->type.def_address = address; /* Setting symbol address if it's not external */  
    }

    new_symbol->symbol_state = symbol_state; /* Setting the symbol state */

    return new_symbol;
}

Symbol symbol_ctor(Symbol sym) 
{
    Symbol * new_sym;

    if (sym == NULL)
    {
        return NULL;
    }
    
    new_sym = &sym; /* Copying address*/
    return *new_sym;
}

void symbol_dtor(Symbol sym) 
{
    if (sym == NULL) 
    {
        return;
    }
    
    if (sym->symbol_state == external_sym) /* Checking whether the symbol is external */
    {
        free(sym->type.instances.external_sym_address_instances); /* Releasing instances array */
    }
    free(sym);
}

int add_instance_to_extern_symbol(Symbol sym, int inst) 
{
    int * temp;

    /* Reallocating integer array */
    temp = (int *)realloc(sym->type.instances.external_sym_address_instances, (sym->type.instances.instances_cnt + REALLOC_CONSTANT) * sizeof(int));

    if (temp == NULL) /* Checking if reallocation successful */
    {
        return 0;
    }

    sym->type.instances.external_sym_address_instances = temp;
    /* Setting all new elements to zero */
    memset(&sym->type.instances.external_sym_address_instances[sym->type.instances.instances_cnt], 0, sizeof(int) * REALLOC_CONSTANT);

    sym->type.instances.external_sym_address_instances[sym->type.instances.instances_cnt++] = inst; /* Adding instance to array */
    return 1;
}

int symbol_cmp(Symbol s1, Symbol s2) 
{
    if (s1 != NULL && s2 != NULL)
        return strcmp(s1->symbol_name, s2->symbol_name) == 0; /* Comparing symbols by their names */
    return 0;
}

void dynamic_arr_destroyer_symbols(Translation_unit * unit) 
{
    /* Freeing all symbol tables of unit */
    free_handler((*unit)->extSymTable->ptr_arr);
    free_handler((*unit)->entSymTable->ptr_arr);
    free_handler((*unit)->extSymTable);
    free_handler((*unit)->entSymTable);
}

void temp_symbol_dtor(Symbol * sym) 
{
    if (sym == NULL || (*sym) == NULL) /* Checking if the pointer to symbol or symbol itself are null */
    {
        return;
    }

    if ((*sym)->symbol_state == external_sym) 
    {
        free((*sym)->type.instances.external_sym_address_instances); /* Destroying instances array */
    }

    free(*sym);
    *sym = NULL; /* Setting ptr to null */
}