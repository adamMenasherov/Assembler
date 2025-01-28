#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

struct node {
    void *data; /* object of the node (Used for generic implementation )*/
    struct node * NextNode; /* Pointer to next node */
};

struct linked_list {
    struct node * lst; /* First node of the list*/
    void *(*ctor)(const void *); /* Constructor for elements */
    void (*dtor)(void *); /* Destructor for elements */
};

typedef struct node *Node; /* A pointer to a node */
typedef struct linked_list *LinkedList; /* A pointer to a list */

/**
 * @brief This function creates a new linked list
 * 
 * @param ctor constructor for elements of list
 * @param dtor destructor for elements of list
 * @return a pointer to a new linked list
 */
LinkedList new_linked_list(void *(*ctor)(const void *), void (*dtor)(void *));
/**
 * @brief This function adds item to the beggining of the linked list (for the O(1) complexity)
 * 
 * @param lst First node of list
 * @param item Item to be added to list
 * @return NULL if adding failed, and item itself if adding was successfull
 */
void * add_to_linked_list(LinkedList lst, const void * item);
/**
 * @brief This function destroyes a list and retrieves all memory that has been allocated
 * 
 * @param list a pointer to a pointer to a list
 */
void linked_list_destroyer(LinkedList * list);
/**
 * @brief The function returns the amount of nodes in the list
 * 
 * @param list The list itself
 * @return amount of nodes
 */
size_t get_size(LinkedList list);

 
#endif

