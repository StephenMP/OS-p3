
/*
	List.h: Defines the interface for a doubly-linked list.
*/

#ifndef __LIST_H
#define __LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "Node.h"

typedef struct list List;
typedef struct list * ListPtr;

struct list {
  int size;
  int poolsize;
  Boolean finish;
  NodePtr head;
  NodePtr tail;
  int (*compareTo)(const void *, const void *);
  char * (*toString)(const void *);
  void (*freeObject)(const void *);
  pthread_mutex_t mutex;
  pthread_cond_t conditionAdd;
  pthread_cond_t conditionRemove;
};

/* prototypes of public methods */

/**
 * Constructor: Allocates a new List object and initializes its members.
 * <code>(size = 0; head = NULL, tail = NULL)</code>.
 *
 * @return a pointer to the allocated list.
 */
ListPtr createList(int(*compareTo)(const void *, const void *),
                   char * (*toString)(const void *),
		           void (*freeObject)(const void *),
                   int poolsize);

/**
 * Frees all elements of the given list and the <code>ListPtr</code> itself.
 * Does nothing if L is <code>NULL</code>.
 *
 * @param L a pointer to a <code>List</code>.
 */
void freeList(const ListPtr L);

/**
 * Adds a node to the front of the list. After this method is called,
 * the given node will be the head of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void addAtFront(ListPtr list, NodePtr node);

/**
 * Adds a node to the rear of the list. After this method is called,
 * the given node will be the tail of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void addAtRear(ListPtr list, NodePtr node);

/**
 * Removes the node from the front of the list (the head node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr removeFront(ListPtr list);

/**
 * Removes the node from the rear of the list (the tail node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr removeRear(ListPtr list);

/**
 * Allows the simulation to stop when producers are done
 *
 * @param L a pointer to a <code>List</code>.
 */
void finishUp(ListPtr L);

#endif /* __LIST_H */

