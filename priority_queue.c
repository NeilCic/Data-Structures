#include <assert.h>
#include <stdlib.h>
#include "priority_queue.h"
#include "srt_list.h"

struct  p_queue_t
{
	srt_list_t *p_queue;
};

p_queue_t *PQCreate(pq_is_before_func is_before, void *param)
{
	p_queue_t *pqueue = NULL;

	assert(is_before);

	pqueue = malloc(sizeof(*pqueue));
	if (pqueue)
	{
		pqueue->p_queue = SrtlistCreate(is_before, param);
		if (!pqueue->p_queue)
		{
			free(pqueue);
			pqueue = NULL;
		}
	}

	return pqueue;
}

void PQDestroy(p_queue_t *pqueue)
{
	assert(pqueue);

	SrtlistDestroy((pqueue->p_queue));
	free(pqueue);
	pqueue = NULL;
}

int PQEnqueue(p_queue_t *pqueue, void *data)
{
	siter_t siter = {0};

	assert(pqueue);

	siter = SrtlistInsert((pqueue->p_queue), data);

	return (SrtlistIsSameIter(siter, SrtlistEnd(pqueue->p_queue)));
}

void *PQDequeue(p_queue_t *pqueue)
{
	assert(pqueue);

	return SrtlistPopFront((pqueue->p_queue));
}

void *PQueuePeek(const p_queue_t *pqueue)
{
	assert(pqueue);

	return SrtlistGetData(SrtlistBegin((pqueue->p_queue)));
}

size_t PQSize(const p_queue_t *pqueue)
{
	assert(pqueue);

	return SrtlistSize((pqueue->p_queue));
}

int PQIsEmpty(const p_queue_t *pqueue)
{
	assert(pqueue);

	return SrtlistIsEmpty((pqueue->p_queue));
}

void *PQErase(p_queue_t *pqueue, pq_is_match_func is_match, void *param)
{
	siter_t siter = {0};
	void *return_data = NULL;

	assert(pqueue);
	assert(is_match);

	siter = SrtlistFindIf(SrtlistBegin((pqueue->p_queue)), 
						SrtlistEnd((pqueue->p_queue)), is_match, param);

	if (SrtlistIsSameIter(siter, SrtlistEnd(pqueue->p_queue)))
	{
		return NULL;
	}

	return_data = SrtlistGetData(siter);
	SrtlistErase(siter);

	return return_data;
}

void PQClear(p_queue_t *pqueue)
{
	assert(pqueue);

	while (!PQIsEmpty(pqueue))
	{
		PQDequeue(pqueue);
	}
}

void PQMerge(p_queue_t *from, p_queue_t *to)
{
	assert(from);
	assert(to);

	SrtlistMerge((from->p_queue), (to->p_queue));

	free(from);
	from = NULL;
}