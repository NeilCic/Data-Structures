#include <assert.h>
#include <stdlib.h>
#include "bstree.h"

/*WITHOUT USING RECURSION*/

#define PARENT(node) (node)->parent
#define PARAM tree->param
#define ISSMALLERCHILD(node) (PARENT(node)->child[0] == node)
#define ISBIGGERCHILD(node) (PARENT(node)->child[1] == node)
#define ROOT tree->root.child[0]
#define NEXT(node) (tree_node_t *)BstNext((bst_iter)node)
#define NEXTITER(iter) ((tree_node_t *)BstNext((bst_iter)iter))
#define PREVITER(iter) ((tree_node_t *)BstPrev((bst_iter)iter))
#define HASRIGHTCHILD(node) (node->child[1]? 1 : 0)
#define HASLEFTCHILD(node) (node->child[0]? 1 : 0)
#define ISLEAF(node) (!node->child[0] && !node->child[1])
#define REWIREPARENTTOGRANDSON(target_node) \
	PARENT(target_node)->child[ISBIGGERCHILD(target_node)] = \
							target_node->child[HASRIGHTCHILD(target_node)]; \
	PARENT(target_node->child[HASRIGHTCHILD(target_node)]) = \
															PARENT(target_node);


typedef struct tree_node_t tree_node_t;

struct tree_node_t
{
	tree_node_t *parent;
	void *data;
	tree_node_t *child[2]; /*0 = left(smaller), 1 = right (larger)*/
};

struct bs_tree_t
{
	tree_node_t root;
	compare_func_t cmp_func;
	void *param;
};

/*
*	This function creates a BST where the root data is always NULL, and the
*	first node is it's left (smaller) child.
*	Arguements = comparator, user parameters.
*	Return value - BStree handle.
*/
bs_tree_t *BstCreate(compare_func_t cmp_func, void *param)
{
	bs_tree_t *tree = NULL;

	assert(cmp_func);

	tree = malloc(sizeof(bs_tree_t));
	if (tree)
	{
		tree->root.parent = NULL;
		tree->root.data = NULL;
		tree->root.child[1] = NULL;
		ROOT = NULL;
		tree->cmp_func = cmp_func;
		PARAM = param;
	}

	return tree;
}

/*
*	This function frees the allocated memory of a tree management struct.
*	Arguments - tree management struct.
*	Return value - none.
*/
void BstDestroy(bs_tree_t *tree)
{
	tree_node_t *node1 = NULL, *node2 = NULL;
	size_t counter = 0;
	assert(tree);

	counter = BstCount(tree);
	if (counter)
	{
		node1 = (tree_node_t *)BstBegin(tree);

		while (counter)
		{
			node2 = NEXT(node1);
			BstRemove((bst_iter)node1);
			node1 = node2;
			--counter;
		}
	}
	
	free(tree);
	tree = NULL;
}

static tree_node_t *CreateNode(void *data)
{
	tree_node_t *node = NULL;

	node = malloc(sizeof(tree_node_t));
	if (!node)
	{
		return NULL;
	}
	node->data = data;
	node->child[0] = NULL;
	node->child[1] = NULL;

	return node;
}

/*
*	return value - iter to the new data
*	arguments - tree management struct, void *data to insert
*	this function receives a tree management struct
*	and insert a new data to it
*/
bst_iter BstInsert(bs_tree_t *tree, void *data)
{
	tree_node_t *node = NULL, *node_parent = NULL;

	assert(tree);
	assert(data); /*cant compare NULL*/

	node = CreateNode(data);
	if (!node)
	{
		return BstEnd(tree);
	}

	if (BstIsEmpty(tree))
	{
		PARENT(node) = &(tree->root);
		ROOT = node;
		
		return ((bst_iter)node);
	}

	node_parent = ROOT;

	while (node_parent->child[(tree->cmp_func(data, node_parent->data, PARAM) 
																		> 0)])
	{
		node_parent = node_parent->child[tree->cmp_func(data, node_parent->data,
																 PARAM) > 0];
	}

	if (tree->cmp_func(data, node_parent->data, PARAM) > 0)
	{
		node_parent->child[1] = node;
	}
	else
	{
		node_parent->child[0] = node;	
	}
	
	PARENT(node) = node_parent;

	return ((bst_iter)node);
}

/*
*	return value - iter to the wanted data
*	arguments - tree management struct, void *data to find
*	this function receives a tree management struct
*	and finds a given value and returns its iter
*/
bst_iter BstFind(bs_tree_t *tree, const void *data)
{
	tree_node_t *node = NULL;

	assert(tree);

	node = ROOT;
	while (node->child[tree->cmp_func(data, node->data, PARAM) > 0])
	{
		node = node->child[tree->cmp_func(data, node->data, PARAM) > 0];
	}

	return (tree->cmp_func(data, node->data, PARAM) ? 
		BstEnd(tree) : (bst_iter)node);
}

/*
*	return value - returns numbe of elements
*	arguments - tree management struct.
*	this function counts the number of elements in the tree.
*/
size_t BstCount(const bs_tree_t *tree)
{
	tree_node_t *iterator = NULL;
	size_t counter = 0;

	assert(tree);

	if (tree->root.child[0])
	{
		iterator = ((tree_node_t *)BstBegin((bs_tree_t *)tree));

		while (iterator->data)
		{
			iterator = NEXTITER(iterator);
			++counter;
		}
	}

	return counter;
}

/*
*	return value - element's data
*	arguments - tree management struct.
*	this function removes given element.
*/

void *BstRemove(bst_iter iter)
{
	tree_node_t *target_node = NULL;
	void *data = NULL;
	tree_node_t *left_child = NULL;

	assert(iter);

	target_node = (tree_node_t *)iter;
	data = target_node->data;

	if (ISLEAF(target_node))
	{
		PARENT(target_node)->child[ISBIGGERCHILD(target_node)] = NULL;
	}
	else if (!(target_node->child[0] && target_node->child[1]))
	{
		if (ISBIGGERCHILD(target_node))
		{
			REWIREPARENTTOGRANDSON(target_node)
		}
		else
		{
			REWIREPARENTTOGRANDSON(target_node)
		}
	}
	else /* (target_node->child[0] && target_node->child[1]])*/
	{
		left_child = target_node->child[0];
		PARENT(target_node)->child[ISBIGGERCHILD(target_node)] = left_child;
		PARENT(left_child) = PARENT(target_node);
		PARENT(target_node->child[1]) = PREVITER(target_node);
		PREVITER(target_node)->child[1] = target_node->child[1];
	}

	free(target_node);
	
	return data;
}

/*
*	return value - success / fail.
*	arguments - tree management struct, action function, param
*	this function performs the given action for each element.
*/
int BstForEach(bst_iter from, bst_iter to, action_func_t act_func, void *param)
{
	tree_node_t *from_node = NULL, *to_node = NULL;
	int func_result = 0;

	assert(from);
	assert(to);
	assert(act_func);

	from_node = (tree_node_t *)from;
	to_node = (tree_node_t *)to;

	while (from_node != to_node && !func_result)
	{
		func_result = act_func(BstGetData((bst_iter)from_node), param);
		from_node = (tree_node_t *)BstNext((bst_iter)from_node);
	}

	return (from_node != to_node);
}

/*
*	return value - empty/not empty.
*	arguments - tree management struct.
*	The function checks if the tree is empty or not.
*/
int BstIsEmpty(const bs_tree_t *tree)
{
	assert(tree);
	return (ROOT ? 0 : 1);
}

/*
*	return value - iterator to the first data.
*	arguments - tree management struct.
*	The function returns the beginning of the tree.
*/
bst_iter BstBegin(bs_tree_t *tree)
{
	tree_node_t *node = NULL;

	assert(tree);

	node = ROOT;
	while (node->child[0])
	{
		node = node->child[0];
	}

	return ((bst_iter)node);
}

/*
*	return value - iterator to the last data.
*	arguments - tree management struct.
*	The function returns the last data.
*/
bst_iter BstEnd(bs_tree_t *tree)
{
	assert(tree);
	return ((bst_iter)&(tree->root));
}

/*
*	return value - next iterator.
*	arguments - tree iteraor.
*	The function returns the next iterator.
*/
bst_iter BstNext(bst_iter iter)
{
	tree_node_t *node = NULL;

	assert(iter);

	node = (tree_node_t *)iter;
	if (HASRIGHTCHILD(node))
	{
		node = node->child[1];
		while (HASLEFTCHILD(node))
		{
			node = node->child[0];
		}

		return ((bst_iter)(node));
	}
	else
	{
		while (ISBIGGERCHILD(node))
		{
			node = PARENT(node);
		}
		
		return ((bst_iter)PARENT(node));
	}
}

/*
*	return value - previous iterator.
*	arguments - tree iteraor.
*	The function returns the previous iterator.
*/
bst_iter BstPrev(bst_iter iter)
{
	tree_node_t *node = NULL;

	assert(iter);

	node = (tree_node_t *)iter;
	if (HASLEFTCHILD(node))
	{
		node = node->child[0];
		while (HASRIGHTCHILD(node))
		{
			node = node->child[1];
		}

		return ((bst_iter)(node));
	}
	else
	{
		while (ISSMALLERCHILD(node))
		{
			node = PARENT(node);
		}

		return ((bst_iter)PARENT(node));
	}
}

/*
*	return value - iterator data.
*	arguments - tree iterator.
*	The function returns the data of given iterator.
*/
void *BstGetData(bst_iter iter)
{
	assert(iter);
	return (((tree_node_t *)iter)->data);
}

int BstIsSameIter(bst_iter iter1, bst_iter iter2)
{
	assert(iter1);
	assert(iter2);
	return (iter1 == iter2);
}