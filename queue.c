#include <assert.h>
#include <stdlib.h>
#include "queue.h"
#include "linked_list.h"


struct queue_t
{
	slist_node_t *head;
	slist_node_t *tail;
};

queue_t *QueueCreate(void)
{
	queue_t *q_t = NULL;
	slist_node_t *dummy = NULL;

	dummy = SlistCreateNode(NULL, NULL);
	if (!dummy)
	{
		return NULL;
	}

	q_t = malloc(sizeof(queue_t));
	if (!q_t)
	{
		dummy = NULL;
		free(dummy);
		
		return NULL;
	}

	q_t->head = dummy;
	q_t->tail = dummy;

	return q_t;
}

int QueueEnqueue(queue_t *queue, void *data)
{
	slist_node_t *new_node = NULL;

	assert(queue);
	assert(data);

	new_node = SlistCreateNode(data, NULL);
	if (!new_node)
	{
		return 1;
	}

	SlistInsert(queue->tail, new_node);
	queue->tail = queue->tail->next;

	return 0;
}

void *QueueDequeue(queue_t *queue) 
{
	void *removed_data = NULL;
	slist_node_t *to_delete = NULL;

	assert(queue);

	removed_data = queue->head->data;
	to_delete = queue->head;
	queue->head = queue->head->next;
	free(to_delete);

	return removed_data;
}

void *QueuePeek(queue_t *queue)
{
	assert(queue);
	assert(!QueueIsEmpty(queue));

	return queue->head->data;
}

size_t QueueSize(const queue_t *queue)
{
	assert(queue);

	return SlistCount(queue->head) - 1;
}

int QueueIsEmpty(const queue_t *queue)
{
	assert(queue);

	return (queue->head == queue->tail);
}

queue_t *QueueAppend(queue_t *to, queue_t *from)
{
	assert(to);
	assert(from);

	to->tail->data = from->head->data;
	to->tail->next = from->head->next;
	to->tail = from->tail;
	from->head->next = NULL;
	from->tail = NULL;

	QueueDestroy(from);

	return to;
}

void QueueDestroy(queue_t *queue)
{
	assert(queue);

	SlistFreeAll(queue->head);
	free(queue);
}