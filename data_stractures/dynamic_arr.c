#include "dynamic_arr.h"
#include "../utils/tools.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>


Dynamic_arr create_arr(void *(*ctor)(const void *), void (*dtor)(void *)) 
{
    Dynamic_arr new_arr = (Dynamic_arr)(handle_malloc(sizeof(struct dynamic_arr))); /* Allocating memory for new dynamic array */

    if (new_arr == NULL) /* Checking if allocation was successful */
    {
        return NULL;
    }

    new_arr->ptr_arr = handle_malloc(DYNAMIC_INITIAL * sizeof(void *));  /* Allocating initial 16 pointers to pointer array */

    if (new_arr->ptr_arr == NULL) /* Checking if allocation was successful*/
    {
        free(new_arr);
        return NULL;
    }

    new_arr->ptr_cnt = DYNAMIC_INITIAL; /* Giving initial 16 pointers */
    new_arr->item_cnt = 0; /* Setting current item count to zero */
    memset(new_arr->ptr_arr, 0, DYNAMIC_INITIAL * sizeof(void *)); /* Setting bytes to 0 */

    /* Setting constructor and destructor */
    new_arr->ctor = ctor; 
    new_arr->dtor = dtor;

    return new_arr; /* Returning new dynamic array */
}


void dynamic_arr_destroyer(Dynamic_arr * arr) 
{
    size_t i;

    if (arr == NULL || (*arr) == NULL) /* Checking whether pointer to array or array itself are null */
    {
        return;
    }

    for (i = 0; i < (*arr)->ptr_cnt; i++) /* Traversing dynamic array */
    {
        if ((*arr)->ptr_arr[i] != NULL) 
        {
            (*arr)->dtor((*arr)->ptr_arr[i]); /* Calling the destructor for each element if it's ain't null */
        }
    }

    free((*arr)->ptr_arr); /* Freeing pointer array */
    (*arr)->ptr_arr = NULL;
    free(*arr); /* Freeing dynamic array */
    *arr = NULL;
}

void * search_dynamic_array(Dynamic_arr arr, void * element, int (*cmp_func)(void *, void *)) 
{
    int i;

    for (i = 0; i < arr->item_cnt; i++) /* Traversing dynamic array */
    {
        if (arr->ptr_arr[i] != NULL && cmp_func(arr->ptr_arr[i], element)) /* If comparison was valid */ 
        {
            return arr->ptr_arr[i];
        }
    }

    return NULL; /* No element was found matching */
}

void * insert_new_element(Dynamic_arr arr, const void * item, size_t index, int flag) 
{
    void ** t;
    size_t size, temp_size, i;

    if (index >= arr->ptr_cnt || arr->ptr_cnt == arr->item_cnt) /* No more pointers available */ 
    {
        arr->item_cnt = arr->ptr_cnt;
        arr->ptr_cnt = 2 * arr->ptr_cnt + index; /* Expanding amount of pointers by 2 plus the index given */
        t = realloc(arr->ptr_arr, arr->ptr_cnt * sizeof(void *)); /* Reallocating */

        if (t == NULL) /* Checking if reallocation successful*/
        {
            return NULL;
        }

        arr->ptr_arr = t;
        temp_size = arr->ptr_cnt - arr->item_cnt; 
        size = temp_size * sizeof(void *);
        memset(&arr->ptr_arr[arr->item_cnt], 0, size); /* Setting all new pointers to NULL */
    }

    if (flag == NEXT_NULL) /* Adding in the next null spot */ 
    {
        for (i = 0; i < arr->ptr_cnt; i++) 
        {
            if (arr->ptr_arr[i] == NULL) 
            {
                arr->ptr_arr[i] = arr->ctor(item);

                if (arr->ptr_arr[i] != NULL) /* Insertion succesfull */
                {
                    arr->item_cnt++;
                    return arr->ptr_arr[i]; /* Returning item if insertion was successful*/
                }
            }
        }
    }

    if (arr->ptr_arr[index] == NULL) 
    {
        arr->ptr_arr[index] = arr->ctor(item);

        if (arr->ptr_arr[index] != NULL) /* Insertion succesfull */
        {
            arr->item_cnt++;
            return arr->ptr_arr[index];
        }
    }
    return NULL;
}