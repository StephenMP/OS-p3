
#ifndef __NODE_H
#define __NODE_H

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

typedef struct node Node;
typedef struct node * NodePtr;

/**
 * A struct to define a node contained in our
 * <code>List</code>
 */
struct node {
	void *obj;
	NodePtr next;
	NodePtr prev;
};

/**
 * Constructor to create and return a <code>Node</code>
 * 
 * @param O a job <code>Object</code>
 */
NodePtr createNode (const void *O);

/**
 * Method to free a node and its contained <code>Object</code>
 * 
 * @param N the node to free
 * @param freeObject the <code>Object</code>'s free function pointer
 */
void freeNode(const NodePtr N, void (*freeObject)(const void *));


#endif /* __NODE_H */
