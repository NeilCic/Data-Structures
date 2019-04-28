#include <assert.h>
#include <stdlib.h>
#include "heap.h"
#include "priority_queue_heap.h"

struct p_queue_t
{
	heap_t *heap;
	void **arr;
};

p_queue_t *PQCreate(pq_comp_func_t cmp_func, size_t elem_num, void *param)
{
	p_queue_t *pqueue = NULL;

	assert(NULL != cmp_func);
	assert(0 < elem_num);

	pqueue = calloc(sizeof(p_queue_t), 1);
	if (NULL == pqueue)
	{
		return NULL;
	}

	pqueue->arr = calloc(elem_num, sizeof(void *));
	if (NULL == pqueue->arr)
	{
		free(pqueue);
		return NULL;
	}

	pqueue->heap = calloc(sizeof(heap_t), 1);
	if (NULL == pqueue->arr)
	{
		free(pqueue->arr);
		free(pqueue);
		return NULL;
	}
	
	HeapInit(pqueue->arr, pqueue->heap, elem_num, cmp_func, param);

	return pqueue;
}

void PQDestroy(p_queue_t *pqueue)
{
	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);

	free(pqueue->heap);
	free(pqueue->arr);
	free(pqueue);
}

void *PQueuePeek(const p_queue_t *pqueue)
{
	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);

	return HeapPeek(pqueue->heap);	
}

void *PQDequeue(p_queue_t *pqueue)
{
	void *returned_data = NULL;

	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);

	returned_data = HeapPeek(pqueue->heap);
	HeapPop(pqueue->heap);

	return returned_data;
}

int PQEnqueue(p_queue_t *pqueue, void *data)
{
	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);
	assert(NULL != data);

	return HeapPush(pqueue->heap, data);
}

size_t PQSize(const p_queue_t *pqueue)
{
	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);

	return HeapSize(pqueue->heap);
}

int PQIsEmpty(const p_queue_t *pqueue)
{
	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);

	return HeapIsEmpty(pqueue->heap);
}

void *PQErase(p_queue_t *pqueue, pq_is_match_func_t is_match, void *param)
{
	assert(NULL != pqueue);
	assert(NULL != pqueue->heap);
	assert(NULL != pqueue->arr);	

	return HeapRemoveReturnData(pqueue->heap, param, is_match, NULL);
}

void PQClear(p_queue_t *pqueue)
{
	assert(pqueue);

	while (!PQIsEmpty(pqueue))
	{
		PQDequeue(pqueue);
	}
}