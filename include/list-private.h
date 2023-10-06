#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#define ENTRY_EQUAL 0
#define ENTRY_LESSER -1
#define ENTRY_GREATER 1
#define ENTRY_CMP_ERROR -2

#include "entry.h"

struct node_t {
	struct entry_t *entry;
	struct node_t  *next; 
};

struct list_t {
	int size;
	struct node_t *head;
};

/* Creates a new node with given entry and next node.
 *
 * Returns the created node's pointer, or the null pointer in case the creation's
 * failed.
 */
struct node_t* node_create(struct entry_t* entry, struct node_t* next);

/*
** Frees up all the memory occupied by a given node
**
** Returns 0 if the operation was successful or -1 in case of error
*/
int node_destroy(struct node_t* node);

#endif
