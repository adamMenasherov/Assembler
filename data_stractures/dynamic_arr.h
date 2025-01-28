#ifndef DYNAMIC_ARR_H
#define DYNAMIC_ARR_H

#define DYNAMIC_INITIAL 16
#include <stdlib.h>

/* Representation of a dynamic array */
struct dynamic_arr 
{
    void ** ptr_arr; /* Array of void objects (for generic implementation )*/
    size_t item_cnt; /* The amount of items that are exists in the array */
    size_t ptr_cnt; /* The amount of pointers (spots) in the array */
    /* Constructor and destructor for items in the array */
    void *(*ctor)(const void *); 
    void (*dtor)(void *);
};


typedef struct dynamic_arr * Dynamic_arr; /* A pointer to a dynamic array */

/**
 * @brief This function creates a new dynamic array. 
 * 
 * @param ctor Constructor for items of the array. 
 * @param dtor Destructor  for items of the array.
 * @return The function returns the dynamic array that has been created.
 */
Dynamic_arr create_arr(void *(*ctor)(const void *), void (*dtor)(void *));
/**
 * @brief This function recieves a dynamic array and destroyes it. It 
 * retrives all the memory that has been allocated to the dynamic array.  
 * 
 * @param arr The dynamic array. 
 */
void dynamic_arr_destroyer(Dynamic_arr * arr);
/**
 * @brief This function is conducting a search in the array to look for a given item.
 * 
 * @param arr The dynamic array. 
 * @param element The element that's being looked for in the array.
 * @param cmp_func pointer to a function that conducts a comparison between two items.
 * @return void* - The item if it was found, or NULL otherwise. 
 */
void * search_dynamic_array(Dynamic_arr arr, void * element, int (*cmp_func)(void *, void *));
/**
 * @brief This function inserts a new element in the dynamic array.
 * 
 * @param arr The dynamic array.
 * @param item The item to be inserted. 
 * @param index The index in which the element will be inserted 
 * @param flag NEXT_NULL to have the element inserted in the next available spot, or any other number for a specific index. 
 * @return returns the new element, or NULL if insertion unsuccesfull.
 */
void * insert_new_element(Dynamic_arr arr, const void * item, size_t index, int flag);

#endif