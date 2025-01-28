#include "linkedlist.h"
#include "../utils/tools.h"



LinkedList new_linked_list(void *(*ctor)(const void *), void (*dtor)(void *)) {
    LinkedList newList = (LinkedList)handle_malloc(sizeof(struct linked_list));  /* Allocating memeory for new list */

    if (newList == NULL) /* Checking if allocation failed */ 
    {
        return NULL;
    }

    newList->lst = NULL; /* Setting first node to null */
    /* Setting constructor and destructor */
    newList->ctor = ctor; 
    newList->dtor = dtor;

    return newList; /* returning list */
}

void * add_to_linked_list(LinkedList list, const void *item) {
    Node node;
    node = (Node)handle_malloc(sizeof(struct node)); /* Allocating memory for new node */

    if (node == NULL) /* Checking if allocation failed */
    {
        return NULL; 
    }

    node->data = list->ctor(item); /* Creating the new element */
    node->NextNode = list->lst; /* Setting the next node to lst */
    list->lst = node; /* Setting lst to current node  */

    return node; /* Returning the node */
}


void linked_list_destroyer(LinkedList *list) 
{
    Node p, t;
    if (list == NULL || *list == NULL) /* Checking whether current pointer to list or list itself are null */ 
    {
        return;
    }

    p = (*list)->lst;
    while (p != NULL) 
    {
        t = p->NextNode; /* Pointing to next node */
        (*list)->dtor(&(p->data)); /* Destroying current node */
        free(p);
        p = t; /* Pointing to next node */
    }

    /* Freeing list */
    free(*list);
    *list = NULL;
}

size_t get_size(LinkedList list) 
{
    Node p;
    size_t i;

    for (p = list->lst, i = 0; p != NULL; p = p->NextNode, i++); /* Counting elements */
    return i;
}