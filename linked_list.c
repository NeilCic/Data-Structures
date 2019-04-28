#include <assert.h> 	/*asserts...*/
#include <stdlib.h>		/*malloc, free, size_t*/
#include "linked_list.h"

static slist_node_t *FindRandomNodeInLoop(slist_node_t *head);

slist_node_t *SlistCreateNode(void *data, slist_node_t *next)
{

	slist_node_t* link = NULL;

	link = malloc(sizeof(slist_node_t));
	if (!link)
	{
		return NULL;
	}

	link->data = data;
	link->next = next;

	return link;
}

void SlistFreeAll(slist_node_t *head)
{
	slist_node_t *temp = head;

	assert(head);

	while (temp)
	{
		head = temp;
		temp = temp->next;
		free(head);
		head = NULL;
	}
}

slist_node_t *SlistInsertAfter(slist_node_t *where, slist_node_t *new_node)
{
	assert(where);
	assert(new_node);

	new_node->next = where->next; 
	where->next = new_node;

	return new_node;
}

static void SwapNodeData(slist_node_t *node1, slist_node_t *node2)
{
	void *temp = NULL;

	temp = node1->data;
	node1->data = node2->data;
	node2->data = temp;
}

slist_node_t *SlistInsert(slist_node_t *where, slist_node_t *new_node)
{
	SwapNodeData(where, new_node);
	SlistInsertAfter(where, new_node);

	return where;
}

slist_node_t *SlistRemoveAfter(slist_node_t *where)
{
	slist_node_t *temp = NULL;

	assert(where);
	assert(where->next);

	if (!where->next)
	{
		return NULL;
	}

	temp = where->next;
	where->next = where->next->next;
	free(temp);

	return where;
}

slist_node_t *SlistRemove(slist_node_t *delete_node)
{
	assert(delete_node);
	assert(delete_node->next);

	delete_node->data = delete_node->next->data;

	return SlistRemoveAfter(delete_node);
}

size_t SlistCount(const slist_node_t *head)
{
	size_t counter = 0;

	assert(head);

	while (NULL != head)
	{
		head = head->next;
		++counter;
	}

	return counter;
}

slist_node_t *SlistFind(slist_node_t *head, cmp_func_t cmp, const void *param)
{
	slist_node_t *temp = head;

	assert(head);
	assert(param);
	assert(cmp);

	while (temp && cmp(temp->data, param))
	{
		temp = temp->next;
	}

	return temp;
}

int SlistForEach(slist_node_t *head, act_func_t act, void *param)
{
	slist_node_t *temp = head;

	assert(head);
	assert(act);

	while (temp && act(temp->data, param))
	{
		temp = temp->next;
	}

	return (temp? 0 : 1);
}

static const slist_node_t *LevelLength(const slist_node_t *node1, const slist_node_t *node2,
												size_t count1, size_t count2)
{
	while (count1 - count2)
	{
		node1 = node1->next;
		--count1;
	}

	return node1;
}

const slist_node_t *SlistFindIntersection(const slist_node_t *head1, 
													const slist_node_t *head2)
{
	size_t count1 = SlistCount(head1), count2 = SlistCount(head2);

	assert(head1);
	assert(head2);

	if (count1 > count2)
	{
		head1 = LevelLength(head1, head2, count1, count2);
	}
	else
	{
		head2 = LevelLength(head2, head1, count2, count1);
	}

	if (head1 == head2)
	{
		return NULL;
	}

	while (head1->next != head2->next)
	{
		head1 = head1->next;
		head2 = head2->next;
	}

	return head1->next;
}

slist_node_t *SlistFlip(slist_node_t *head)
{
	slist_node_t *previous = NULL, *next = NULL;

	assert(head);

	previous = head;
	next = head->next;

	head->next = NULL;

	while (next)
	{
		previous = next;
		next = previous->next;

		previous->next = head;
		head = previous;
	}

	return head;
}

static slist_node_t *RecursiveFlipAid(slist_node_t *head, slist_node_t *next)
{
	slist_node_t *next_next = NULL;

	if (NULL != next->next)
	{
		next_next = next->next;
		next->next = head;
		
		return RecursiveFlipAid(next, next_next);
	}
	else
	{
		next->next = head;
		
		return next;
	}
}

slist_node_t *SlistFlipRecursive(slist_node_t *head)
{
	slist_node_t *next = NULL;

	assert(head);

	if (NULL == head->next)
	{
		return head;
	}

	next = head->next;
	head->next = NULL;

	return RecursiveFlipAid(head, next);
}

int SlistHasLoop(const slist_node_t *head)
{
	const slist_node_t *slow = head, *fast = head->next;

	assert(head);

	while (slow != fast && fast && fast->next && fast->next->next)
	{
		slow = slow->next;
		fast = fast->next->next;
	}

	return (slow == fast);
}

/*assuming head1 is the main branch and we disconnect head2*/
int SlistResolveIntersection(slist_node_t *head1, slist_node_t *head2)
{
	assert(head1);
	assert(head2);

	if (!head1)
	{
		return 1; /*returning faliure since there was no intersection*/
	}

	while (SlistFindIntersection(head1, head2) != head2->next)
	{
		head2 = head2->next;
	}

	head2->next = NULL;

	return 0;
}

int SlistResolveLoop(slist_node_t *head)
{
	slist_node_t *node1 = NULL, *node2 = NULL;

	assert(head);

	if (!SlistHasLoop(head))
	{
		return 1;
	}

	node1 = FindRandomNodeInLoop(head);

	if (node1->next == head)
	{
		node1->next = NULL;
		
		return 0;
	}
	
	node2 = node1->next;
	node1->next = NULL; /*severing loop at an unknown spot*/

	/*incase there's an intersection*/
	if (SlistFindIntersection(head, node2))
	{
		SlistResolveIntersection(head, node2);
		node1->next = node2;	/*reconnecting severed nodes*/
	}

	return 0;
}

static slist_node_t *FindRandomNodeInLoop(slist_node_t *head)
{
	slist_node_t *slow = head, *fast = head->next;

	while (slow != fast && fast && fast->next)
	{
		slow = slow->next;
		fast = fast->next->next;
	}

	return slow;
}

