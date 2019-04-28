#include <assert.h>
#include <stdlib.h>
#include "dl_list.h"
#include <stdio.h>

typedef struct dlist_node_t node_t;

struct dlist_node_t
{
	void *data;
	node_t *next;
	node_t *prev;
};

struct dlist_t
{
	node_t head;
	node_t tail;
};



/*****************************************************************************/
dlist_t *DlistCreateList(void)
{
	dlist_t *dlist= malloc(sizeof(dlist_t));
	if (!dlist)
	{
		return NULL;
	}

	dlist->head.next = &dlist->tail;
	dlist->tail.prev = &dlist->head;
	dlist->head.prev = NULL;
	dlist->tail.next = NULL;
	dlist->head.data = NULL;
	dlist->tail.data = NULL;

	return dlist;
}

iter_t DlistBegin(dlist_t *dlist)
{
	assert(dlist);
	return (iter_t)dlist->head.next;
}

iter_t DlistEnd(dlist_t *dlist)
{
	assert(dlist);
	return (iter_t)&dlist->tail;
}

iter_t DlistInsert(dlist_t *dlist, iter_t iter, void *data)
{
	node_t *new_node = NULL;

	assert(dlist);
	assert(iter);

	new_node = malloc(sizeof(node_t));
	if (!new_node)
	{
		return DlistEnd(dlist);
	}
	
	new_node->data = data;

	/*rearranging pointers*/
	new_node->prev = ((node_t *)iter)->prev;
	new_node->next = ((node_t *)iter);
	((node_t *)iter)->prev->next = new_node;
	((node_t *)iter)->prev = new_node;

	return (iter_t)new_node;
}

iter_t DlistErase(iter_t iter)
{
	node_t *next_node = NULL, *prev_node = NULL;

	assert(iter);

	next_node = ((node_t *)iter)->next;
	prev_node = ((node_t *)iter)->prev;

	((node_t *)iter)->next = NULL;
	((node_t *)iter)->prev = NULL;

	prev_node->next = next_node;
	next_node->prev = prev_node;

	free((node_t *)iter);

	return (iter_t)next_node;
}

iter_t DlistFind(iter_t from, iter_t to, is_match_func_t is_match, 
															const void *param)
{
	assert(from);
	assert(to);
	assert(is_match);

	while (!IsSameIter(from,to) && !is_match(DlistGetData(from), param))
	{
		from = DlistNext(from);
	}

	return from;
}

int DlistForEach(iter_t from, iter_t to, act_func_t act, void *param)
{
	int status = 0;

	assert(from);
	assert(to);
	assert(act);

	while (!IsSameIter(from, to) && !status)
	{
		status = act(DlistGetData(from), param);
		from = DlistNext(from);
	}

	return status;
}

size_t DlistCount(const dlist_t *dlist)
{
	node_t *first_node = NULL;
	size_t counter = 0;

	assert(dlist);

	first_node = dlist->head.next;

	while (first_node->next)
	{
		++counter;
		first_node = first_node->next;
	}

	return counter;
}

int DlistIsEmpty(const dlist_t *dlist)
{
	assert(dlist);

	return (dlist->head.next == &dlist->tail);
}

iter_t DlistPushFront(dlist_t *dlist, void *data)
{
	node_t *new_first_node = NULL;

	assert(dlist);

	new_first_node = (node_t *)DlistInsert(dlist, (iter_t)dlist->head.next, 
																		data);

	return (iter_t)new_first_node;
}

iter_t DlistPushBack(dlist_t *dlist, void *data)
{
	node_t *new_last_node = NULL;

	assert(dlist);

	new_last_node = (node_t *)DlistInsert(dlist, (iter_t)&dlist->tail, data);

	return (iter_t)new_last_node;
}

void *DlistPopFront(dlist_t *dlist)
{
	void *data = NULL;

	assert(dlist);

	data = dlist->head.next->data;

	DlistErase((iter_t)dlist->head.next);

	return data; 
}

void *DlistPopBack(dlist_t *dlist)
{
	void *data = NULL;

	assert(dlist);

	data = dlist->tail.prev->data;

	DlistErase((iter_t)dlist->tail.prev);

	return data;  
}

iter_t DlistNext(iter_t iter)
{
	assert(iter);

	return (iter_t)(((node_t *)iter)->next);
}

iter_t DlistPrev(iter_t iter)
{
	assert(iter);
	
	return (iter_t)(((node_t *)iter)->prev);
}

int IsSameIter(iter_t iter1, iter_t iter2)
{
	assert(iter1);
	assert(iter2);
	
	return (iter1 == iter2);
}

void *DlistGetData(iter_t iter)
{
	assert(iter);

	return ((node_t *)iter)->data;
}

void DlistSplice(iter_t dest, iter_t from, iter_t to)
{
	node_t *dest_node = NULL, *from_node = NULL, *to_node = NULL,
							 *before_from = NULL, *after_dest = NULL;

	assert(dest);
	assert(from);
	assert(to);

	dest_node = ((node_t *)dest);
	from_node = ((node_t *)from);
	to_node = ((node_t *)to);

	before_from = from_node->prev;
	after_dest = dest_node->next;

	dest_node->next 	= from_node;
	before_from->next 	= to_node;
	from_node->prev		= dest_node;
	to_node->prev->next = after_dest;
	after_dest->prev 	= to_node->prev;
	to_node->prev 		= before_from;
}

void DlistDestroy(dlist_t *dlist)
{
	node_t *delete_node = NULL, *next_node = NULL;

	assert(dlist);

	delete_node = dlist->head.next;
	next_node = dlist->head.next->next;

	while (delete_node->next)
	{
		free(delete_node);
		delete_node = next_node;
		next_node = next_node->next;
	}

	next_node = NULL;
	free(dlist);
	dlist = NULL;
}

