#include <stdio.h>
#include <stdlib.h>
#include "List.h"

/*

   list.c
   Contains functions to manipulate a doubly-linked list.

 */

/* private methods */
static void print(const NodePtr node, char * (*toString)(const void *));
static void print_jobs(const NodePtr node, char * (*toString)(const void *));

ListPtr createList(int (*compareTo)(const void *, const void *),
		   char * (*toString)(const void *), void (*freeObject)(const void *))
{
	/* Initialize the list */
	ListPtr list;
	list = (ListPtr)malloc(sizeof(List));

	/* Setup list instance data */
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	list->compareTo = compareTo;
	list->toString = toString;
	list->freeObject = freeObject;

	/* Throw back the pointer to it */
	return list;
}

void freeList(const ListPtr list)
{
	/* Check corner case(s) */
	if (list == NULL)
		return;

	/* Setup local data to allow list traversal */
	NodePtr curr = list->head;
	NodePtr temp;

	/* Traverse list keeping ref to one node ahead and free current node */
	while (curr != NULL) {
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

int getSize(const ListPtr list)
{
	return list->size;
}

Boolean isEmpty(const ListPtr list)
{
	if (list->size == 0)
		return TRUE;
	else
		return FALSE;
}

void addAtFront(ListPtr list, NodePtr node)
{
	/* Check corner case(s) */
	if (list == NULL || node == NULL)
		return;

	/* Increment list size */
	list->size++;

	/* Update nodes next/prev references */
	node->next = list->head;
	node->prev = NULL;

	/* No current head */
	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
	}else {
		list->head->prev = node;
		list->head = node;
	}
}

void addAtRear(ListPtr list, NodePtr node)
{
	/* Process is the same as above, only with the tail node */
	if (list == NULL || node == NULL)
		return;

	list->size++;
	node->next = NULL;
	node->prev = list->tail;
	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
	}else {
		list->tail->next = node;
		list->tail = node;
	}
}

NodePtr removeFront(ListPtr list)
{
	/* Check corner case(s) */
	if (list == NULL || list->head == NULL)
		return NULL;

	/* Ref to node to delete */
	NodePtr headNode = list->head;

	/* Removing handles differently based on # of nodes in list*/
	if (list->size > 1) {
		NodePtr afterHeadNode = list->head->next;
		afterHeadNode->prev = NULL;
		list->head = afterHeadNode;
	}else {
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

NodePtr removeRear(ListPtr list)
{
	/* Same as removeFront, only with tail node */
	if (list == NULL || list->head == NULL)
		return NULL;

	NodePtr tailNode = list->tail;

	if (list->size > 1) {
		NodePtr beforeTail = list->tail->prev;
		beforeTail->next = NULL;
		list->tail = beforeTail;
	}else {
		list->head = NULL;
		list->tail = NULL;
	}

	tailNode->next = NULL;
	tailNode->prev = NULL;
	list->size--;

	return tailNode;
}

NodePtr removeNode(ListPtr list, NodePtr node)
{
	/* Check corner case(s) */
	if (list == NULL || list->head == NULL || node == NULL)
		return NULL;

	/* Use search to check that node is in list */
	NodePtr result = search(list, node->obj);

	/* Removal is handled based on specific case */
	if (result == list->head)
		return removeFront(list);
	if (result == list->tail)
		return removeRear(list);
	if (result == NULL)
		return NULL;

	NodePtr next = result->next;
	NodePtr prev = result->prev;

	next->prev = prev;
	prev->next = next;

	list->size--;

	return result;
}

NodePtr search(const ListPtr list, const void *obj)
{
	/* Check corner case(s) */
	if (list == NULL || list->head == NULL || obj == NULL)
		return NULL;

	/* Since list is doubly, we can traverse both forward and backward to search */
	NodePtr hCurr = list->head;
	NodePtr tCurr = list->tail;
	int i;
	for (i = 0; i <= (list->size) / 2; i++) {
		if (list->compareTo(hCurr->obj, obj) == TRUE)
			return hCurr;
		if (list->compareTo(tCurr->obj, obj) == TRUE)
			return tCurr;

		hCurr = hCurr->next;
		tCurr = tCurr->prev;
	}

	return NULL;
}

NodePtr getHead(ListPtr list)
{
	return list->head;
}

void reverseList(ListPtr list)
{
	/* Check corner case(s) */
	if (list == NULL || list->head == NULL)
		return;

	/* Setup references */
	NodePtr temp = NULL;
	NodePtr curr = list->head;
	NodePtr hRef = list->head;

	/*
	 * 1) Use temp to hold reference to curr's prev
	 * 2) Reassign curr's prev to be it's next (reverse what it's referencing
	 * 3) Reassign curr's next to temp (curr's prev, reversing it's reference)
	 * 4) Move 'forward' (curr's prev points to the next node in the list)
	 */
	while (curr != NULL) {
		temp = curr->prev;
		curr->prev = curr->next;
		curr->next = temp;
		curr = curr->prev;
	}

	/* Since curr can be null prior to reassigning temp, we need to handle it */
	if (temp != NULL) {
		list->head = temp->prev;
		list->tail = hRef;
	}
}

void printJobs(const ListPtr list)
{
	if (list)
		print_jobs(list->head, list->toString);
}

void printList(const ListPtr list)
{
	if (list)
		print(list->head, list->toString);
}

static void print(const NodePtr node, char * (*toString)(const void *))
{
	int count = 0;
	char *output;
	NodePtr temp = node;
	while (temp) {
		output = (*toString)(temp->obj);
		printf(" %s -->", output);
		free(output);
		temp = temp->next;
		count++;
		if ((count % 6) == 0)
			printf("\n");
	}
	printf(" NULL \n");
}

static void print_jobs(const NodePtr node, char * (*toString)(const void *))
{
	int count = 0;
	char *output;
	NodePtr temp = node;
	while (temp) {
		output = (*toString)(temp->obj);
		printf("%s\n", output);
		free(output);
		temp = temp->next;
		count++;
		if ((count % 6) == 0)
			printf("\n");
	}
}

