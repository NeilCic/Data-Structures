#include <stdlib.h>
#include <assert.h>
#include "srt_list.h"
#include "dl_list.h"

#define siter_t srt_list_iter_t

typedef struct srt_list_info info_t;

typedef struct srt_list_t
{
	dlist_t *dlist;
#ifndef NDEBUG
	size_t list_id;
#endif
	void *param;
	int(*is_before)(const void *data1, const void *data2, void *param);
}srt_list_t;


srt_list_t *SrtlistCreate(srt_is_before_func_t is_before, void *param)
{
	srt_list_t *new_srt = NULL;

	assert(is_before);

	new_srt = malloc(sizeof(*new_srt));
	if (new_srt)
	{
		new_srt->is_before = is_before;
		new_srt->param = param;

#ifndef NDEBUG
	new_srt->list_id = (size_t)&new_srt;
#endif
		
		new_srt->dlist = DlistCreateList();
		if (!new_srt->dlist)
		{
			free(new_srt);
			new_srt = NULL;
		}
	}

	return new_srt;
}

void SrtlistDestroy(srt_list_t *srt_list)
{
	assert(srt_list);

	DlistDestroy(srt_list->dlist);
	srt_list->dlist = NULL;

	free(srt_list);
	srt_list = NULL;
}

siter_t SrtlistInsert(srt_list_t *srt_list, void *data)
{
	siter_t siter = {0};

	assert(srt_list);

	siter = SrtlistBegin(srt_list);

	while (!SrtlistIsSameIter(siter, SrtlistEnd(srt_list)) &&
			!srt_list->is_before(data, SrtlistGetData(siter), NULL))
	{
		siter = SrtlistNext(siter);
	}

	siter.info = (info_t *)DlistInsert(srt_list->dlist, 
													(iter_t)siter.info, data);

#ifndef NDEBUG
	siter.list_id = srt_list->list_id;
#endif

	return siter;
}

siter_t SrtlistErase(siter_t where)
{
	assert(where.info);

	where.info = (info_t *)DlistErase((iter_t)where.info);

	return where;
}

size_t SrtlistSize(const srt_list_t *srt_list)
{
	assert(srt_list);

	return DlistCount(srt_list->dlist);
}

int SrtlistIsEmpty(const srt_list_t *srt_list)
{
	assert(srt_list);

	return DlistIsEmpty(srt_list->dlist);
}

void *SrtlistPopFront(srt_list_t *srt_list)
{
	assert(srt_list);

	return DlistPopFront(srt_list->dlist);
}

void *SrtlistPopBack(srt_list_t *srt_list)
{
	assert(srt_list);

	return DlistPopBack(srt_list->dlist);
}

siter_t SrtlistBegin(srt_list_t *srt_list)
{
	siter_t siter = {0};

	assert(srt_list);

	#ifndef NDEBUG
		siter.list_id = srt_list->list_id;
	#endif

	siter.info = (info_t *)DlistBegin(srt_list->dlist);

	return siter;
}

siter_t SrtlistEnd(srt_list_t *srt_list)
{
	siter_t siter = {0};

	assert(srt_list);

	#ifndef NDEBUG
		siter.list_id = srt_list->list_id;
	#endif

	siter.info = (info_t*)DlistEnd(srt_list->dlist);

	return siter;
}

siter_t SrtlistNext(siter_t where)
{
	assert(where.info);

	where.info = (info_t *)DlistNext((iter_t)where.info);

	return where;
}

siter_t SrtlistPrev(siter_t where)
{
	assert(where.info);

	where.info = (info_t *)DlistPrev((iter_t)where.info);

	return where;
}

int SrtlistIsSameIter(siter_t iter1, siter_t iter2)
{
	return IsSameIter((iter_t)iter1.info, (iter_t)iter2.info);
}

void *SrtlistGetData(siter_t where)
{
	assert(where.info);

	return DlistGetData((iter_t)where.info);
}

srt_list_iter_t SrtlistFindIf(siter_t from, siter_t to, 
									is_match_func_t is_match, const void *data)
{
	assert(from.info);
	assert(to.info);
	assert(is_match);

	while (!SrtlistIsSameIter(from, to) && 
		!is_match(data,SrtlistGetData(from)))
	{
		from = SrtlistNext(from);
	}

	return from;
}

siter_t SrtlistFind(srt_list_t *srt_list, const void *data)
{
	siter_t siter = {0};

	assert(srt_list);
	assert(data);

	#ifndef NDEBUG
		siter.list_id = srt_list->list_id;
	#endif

	siter = SrtlistPrev(SrtlistEnd(srt_list));

	while (!SrtlistIsSameIter(siter, SrtlistBegin(srt_list)) &&
			srt_list->is_before(data, SrtlistGetData(siter), srt_list->param))
	{
		siter = SrtlistPrev(siter);
	}

	return siter;
}

int SrtlistForEach(siter_t from, siter_t to, act_func_t act, void *act_param)
{
	int res = 0;

	while (!SrtlistIsSameIter(from, to) && !res)
	{
		res = act(SrtlistGetData(from), act_param);
		from = SrtlistNext(from);
	}

	return res;
}

void SrtlistMerge(srt_list_t *from, srt_list_t *to)
{
	siter_t from_begin = {0}, start_copy = {0}, end_copy = {0};

	assert(from);
	assert(to);

	#ifndef NDEBUG
		from_begin.list_id = to->list_id;
		start_copy.list_id = to->list_id;
		end_copy.list_id = to->list_id;
	#endif

	/*cut&paste first and last elements from "from" list to "to"*/
	SrtlistInsert(to,SrtlistGetData(SrtlistBegin(from)));
	SrtlistInsert(to,SrtlistGetData(SrtlistPrev(SrtlistEnd(from))));
	SrtlistPopBack(from);
	SrtlistPopFront(from);

	from_begin = SrtlistBegin(from);

	/*
	while explanation:
	stage 1) finding in "to"  list first value from "from"
												-> gives dest for splice.
	stage 2) using value from stage 1 to search the "from" list for finish
	 			position for splice.
	stage 3) splicing using the stage 1 as dest, and start and end copy.
	*/

	while (!SrtlistIsSameIter(SrtlistBegin(from), SrtlistEnd(from)))
	{
		/*finding destination for splice in "to" list*/
		start_copy = SrtlistNext(SrtlistFind(to,SrtlistGetData(from_begin)));

		/*backing one up because of how the find and splice functions work*/
		start_copy = SrtlistPrev(start_copy);

		/*finding finish position for splice*/
		end_copy = SrtlistNext(SrtlistFind(from,
									SrtlistGetData(SrtlistNext(start_copy))));

		DlistSplice(((iter_t)start_copy.info), ((iter_t)from_begin.info), 
													((iter_t)end_copy.info));
		/*resetting the start position for splice*/
		 from_begin = end_copy;
	}
	
		 SrtlistDestroy(from);
}

