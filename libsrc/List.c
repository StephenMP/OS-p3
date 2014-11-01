#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "List.h"

/*
   list.c
   Contains functions to manipulate a doubly-linked list.
 */

/*****************************/
/** Constructor and free-er **/
/*****************************/
ListPtr createList(int (*compareTo)(const void *, const void *),
                   char * (*toString)(const void *),
                   void (*freeObject)(const void *),
                   int poolsize)
{
	/* Initialize the list */
	ListPtr list;
	list = (ListPtr)malloc(sizeof(List));

	/* Setup list instance data */
	list->size = 0;
	list->poolsize = poolsize;
	list->finish = FALSE;
	list->head = NULL;
	list->tail = NULL;
	list->compareTo = compareTo;
	list->toString = toString;
	list->freeObject = freeObject;

	/* Initialize monitor */
	pthread_mutex_init(&(list->mutex), NULL);
	pthread_cond_init(&(list->conditionAdd), NULL);
	pthread_cond_init(&(list->conditionRemove), NULL);

	/* Throw back the pointer to it */
	return list;
}

/*************************************************/
/** Static functions which actually do the work **/
/*************************************************/
void _freeList(const ListPtr list)
{
	/* Check corner case(s) */
	if(list == NULL)
		return;

	/* Setup local data to allow list traversal */
	NodePtr curr = list->head;
	NodePtr temp;

	/* Traverse list keeping ref to one node ahead and free current node */
	while(curr != NULL) {
		temp = curr;
		curr = curr->next;
		freeNode(temp, list->freeObject);
	}

	/* Clear instance data (just a formality to keep garbage clean) */
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	/* Free the list pointer */
	free(list);
}

static Boolean _isEmpty(const ListPtr list)
{
	return (list->size == 0);
}

static Boolean _isFull(ListPtr list)
{
	return (list->size == list->poolsize);
}

static void _addAtFront(ListPtr list, NodePtr node)
{
	/* Check corner case(s) */
	if(list == NULL || node == NULL)
		return;

	/* Increment list size */
	list->size++;

	/* Update nodes next/prev references */
	node->next = list->head;
	node->prev = NULL;

	/* No current head */
	if(list->head == NULL) {
		list->head = node;
		list->tail = node;
	} else {
		list->head->prev = node;
		list->head = node;
	}
}

static void _addAtRear(ListPtr list, NodePtr node)
{
	/* Process is the same as above, only with the tail node */
	if(list == NULL || node == NULL)
		return;

	list->size++;
	node->next = NULL;
	node->prev = list->tail;
	if(list->head == NULL) {
		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}
}

static NodePtr _removeFront(ListPtr list)
{
	/* Check corner case(s) */
	if(list == NULL || list->head == NULL)
		return NULL;

	/* Ref to node to delete */
	NodePtr headNode = list->head;

	/* Removing handles differently based on # of nodes in list*/
	if(list->size > 1) {
		NodePtr afterHeadNode = list->head->next;
		afterHeadNode->prev = NULL;
		list->head = afterHeadNode;
	} else {
		list->head = NULL;
		list->tail = NULL;
	}

	/* Clear node instance data (avoid access to other nodes post removal) */
	headNode->next = NULL;
	headNode->prev = NULL;

	/* Decrement list size since we are removing */
	list->size--;

	/* Give the pointer back */
	return headNode;
}

static NodePtr _removeRear(ListPtr list)
{
	/* Same as removeFront, only with tail node */
	if(list == NULL || list->head == NULL)
		return NULL;

	NodePtr tailNode = list->tail;

	if(list->size > 1) {
		NodePtr beforeTail = list->tail->prev;
		beforeTail->next = NULL;
		list->tail = beforeTail;
	} else {
		list->head = NULL;
		list->tail = NULL;
	}

	tailNode->next = NULL;
	tailNode->prev = NULL;
	list->size--;

	return tailNode;
}

/*****************************************************************/
/** Non-static functions acting as a thread monitor via wrapper **/
/*****************************************************************/
void freeList(const ListPtr list)
{
	/* Make handles to our pthread stuff */
	pthread_mutex_t m;
	pthread_cond_t ca;
	pthread_cond_t cr;

	/* Lock it down */
	pthread_mutex_lock(&(list->mutex));

	/* Get handle to our pthread stuff */
	m = list->mutex;
	ca = list->conditionAdd;
	cr = list->conditionRemove;

	/* Free our list */
	_freeList(list);

	/* Unlock it */
	pthread_mutex_unlock(&(list->mutex));

	/* Free the pthread stuff */
	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&ca);
	pthread_cond_destroy(&cr);
}

void addAtFront(ListPtr list, NodePtr node)
{
	/* Lock it down and wait if we are at poolsize */
	pthread_mutex_lock(&(list->mutex));
	while(_isFull(list))
		pthread_cond_wait(&(list->conditionAdd), &(list->mutex));

	/* Add the node */
	_addAtFront(list, node);

	/* Signal consumers and unlock */
	pthread_cond_signal(&(list->conditionRemove));
	pthread_mutex_unlock(&(list->mutex));
}

void addAtRear(ListPtr list, NodePtr node)
{
	/* Same thing as above */
	pthread_mutex_lock(&(list->mutex));
	while(_isFull(list))
		pthread_cond_wait(&(list->conditionAdd), &(list->mutex));

	_addAtRear(list, node);

	pthread_cond_signal(&(list->conditionRemove));
	pthread_mutex_unlock(&(list->mutex));
}

NodePtr removeFront(ListPtr list)
{
	/* Make handle to the returned node */
	NodePtr result = NULL;

	/* Lock it and wait while we are empty or not yet finished */
	pthread_mutex_lock(&(list->mutex));
	while(_isEmpty(list) && !list->finish)
		pthread_cond_wait(&(list->conditionRemove), &(list->mutex));

	/* Remove and assign the node */
	result = _removeFront(list);

	/* Unlock and signal producers */
	pthread_cond_signal(&(list->conditionAdd));
	pthread_mutex_unlock(&(list->mutex));

	/* Return the node */
	return result;
}

NodePtr removeRear(ListPtr list)
{
	/* Same thing as above */
	NodePtr result = NULL;

	pthread_mutex_lock(&(list->mutex));
	while(_isEmpty(list) && !list->finish)
		pthread_cond_wait(&(list->conditionRemove), &(list->mutex));

	result = _removeRear(list);

	pthread_cond_signal(&(list->conditionAdd));
	pthread_mutex_unlock(&(list->mutex));

	return result;
}

void finishUp(ListPtr list)
{
	/* Lock it down (not necessary here, but good practice) */
	pthread_mutex_lock(&(list->mutex));

	/* Let consumers know they can go ahead and finish up*/
	list->finish = TRUE;
	pthread_cond_broadcast(&(list->conditionRemove));

	/* Unlock it */
	pthread_mutex_unlock(&(list->mutex));
}

