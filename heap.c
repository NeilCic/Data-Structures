#include <assert.h>
#include <string.h>
#include "heap.h"

#define PARAM heap->param
#define LEFT_CHILD_OFFSET(offset) (2*(offset)+1)
#define LEFT_CHILD_DATA(offset) (heap->arr[LEFT_CHILD_OFFSET(offset)])
#define RIGHT_CHILD_OFFSET(offset) (2*(offset)+2)
#define RIGHT_CHILD_DATA(offset) (heap->arr[RIGHT_CHILD_OFFSET(offset)])
#define PARENT_OFFSET(offset) (((offset)-1)/2)
#define PARENT_DATA(offset) (heap->arr[PARENT_OFFSET(offset)])
#define ELEMENT_DATA(offset) heap->arr[offset]
#define COMPARE_CHILD_TO_PARENT(offset1) \
		heap->cmp_func(ELEMENT_DATA(offset1), PARENT_DATA(offset1), PARAM)
#define COMPARE(data1, data2) heap->cmp_func(data1, data2, PARAM)


enum return_status
{
	SUCCESS,
	DUPLICATE,
	FULL_HEAP,
	EMPTY_HEAP
};

void HeapInit(void **arr, heap_t *heap, size_t num_of_elements, 
									heap_comp_func_t cmp_func, void *param)
{
	assert(heap);
	assert(NULL != arr);
	assert(num_of_elements > 0);
	assert(NULL != cmp_func);

	heap->arr = arr;
	heap->num_of_elements = num_of_elements;
	heap->cmp_func = cmp_func;
	PARAM = param;
}

/*
* push new data to the heap.
* return value: 0 if succeed, 1 if failed (heap is full).
* Arguments: heap , data to insert
*/
static void SwapData(void **data1, void **data2)
{
	void *temp = NULL;

	assert(*data1);
	assert(*data2);

	temp = *data1;
	*data1 = *data2;
	*data2 = temp;
}


static void HeapifyUp(heap_t *heap, size_t data_offset)
{
	assert(heap);
	assert(0 <= data_offset);
	assert(data_offset < heap->num_of_elements);

	while (0 != data_offset && 0 < COMPARE_CHILD_TO_PARENT(data_offset))
	{
		SwapData(&(heap->arr[data_offset]), &(PARENT_DATA(data_offset)));
		data_offset = PARENT_OFFSET(data_offset);
	}
}


int HeapPush(heap_t *heap, void *data)
{
	size_t element_offset = 0;

	assert(heap);
	assert(data);

	if (heap->num_of_elements <= HeapSize(heap))
	{
		return FULL_HEAP;
	}

	while (NULL != ELEMENT_DATA(element_offset))
	{
		if (0 == COMPARE(heap->arr[element_offset], data))
		{
			return DUPLICATE;
		}

		++element_offset;
	}

	heap->arr[element_offset] = data;
	HeapifyUp(heap, element_offset);

	return SUCCESS;
}

static size_t FarthestDataOffset(heap_t *heap, size_t element_offset)
{
	void *right_data = RIGHT_CHILD_DATA(element_offset);
	void *left_data = LEFT_CHILD_DATA(element_offset);

	return COMPARE(right_data, left_data) > 0? 
		RIGHT_CHILD_OFFSET(element_offset) : LEFT_CHILD_OFFSET(element_offset);
}

static size_t FindLastLeaf(heap_t *heap, size_t last_leaf_offset)
{
	while (NULL != ELEMENT_DATA(last_leaf_offset))
	{
		++last_leaf_offset;
	}
	--last_leaf_offset;

	return last_leaf_offset;
}


static void HeapifyDown(heap_t *heap, size_t last_leaf_offset, 
													size_t farther_child_offset)
{
	while (NULL != LEFT_CHILD_DATA(last_leaf_offset))
	{
		if (NULL != RIGHT_CHILD_DATA(last_leaf_offset))
		{
			farther_child_offset = FarthestDataOffset(heap, last_leaf_offset);
			SwapData(&(heap->arr[last_leaf_offset]), 
											&heap->arr[farther_child_offset]);
			last_leaf_offset = farther_child_offset;
		}
		else
		{
			SwapData(&(heap->arr[last_leaf_offset]), 
											&LEFT_CHILD_DATA(last_leaf_offset));
			last_leaf_offset = LEFT_CHILD_OFFSET(last_leaf_offset);
		}
	}
}


/*
* pop the first data from the heap,
* and fix the heap by the new root.
* Arguments: heap.
* Return value: none.
*/
void HeapPop(heap_t *heap)
{
	size_t element_offset = 0;
	size_t last_leaf_offset = 0;
	size_t farther_child_offset = 0;

	assert(heap);

	if (HeapIsEmpty(heap))
	{
		return;
	}

	last_leaf_offset = FindLastLeaf(heap, last_leaf_offset);
	SwapData(&(heap->arr[element_offset]), &heap->arr[last_leaf_offset]);
	ELEMENT_DATA(last_leaf_offset) = NULL;

	HeapifyDown(heap, last_leaf_offset, farther_child_offset);
}

/*
* peek to the first element on the heap root.
* Arguments: heap.
* Return value: heaps peek value.
*/
void *HeapPeek(const heap_t *heap)
{
	assert(heap);

	return (*(heap->arr));
}

/*
* calculate the heap size.
* Arguments: heap.
* Return value: heap size.
*/
size_t HeapSize(const heap_t *heap)
{
	size_t counter = 0;

	assert(heap);

	while (counter < heap->num_of_elements && NULL != heap->arr[counter])
	{
		++counter;
	}

	return counter;
}


/*
* determine if the heap is empty.
* Arguments: heap.
* Return value: 1 - empty, 0 - not empty.
*/
int HeapIsEmpty(const heap_t *heap)
{
	assert(heap);

	return (NULL == heap->arr[0]);
}

static size_t FindRemoveOffset(heap_t *heap, size_t remove_offset,
				const void *data, is_match_func_t is_match, const void *param)
{
	(void)param;
	while (!(is_match((void *)data, ELEMENT_DATA(remove_offset))))
	{
		++remove_offset;
	}

	return remove_offset;
}

/*
* remove a specific element from the heap.
* Arguments: heap, the data to remove, is match fucntion, and a parameter.
* Return value: 0 - success , 1 - failure.
*/
int HeapRemove(heap_t *heap, const void *data, is_match_func_t is_match,
			  												const void *param)
{
	size_t element_offset = 0;
	size_t last_leaf_offset = 0;
	size_t farther_child_offset = 0;

	assert(heap);
	assert(data);
	assert(is_match);

	if (0 == HeapSize(heap))
	{
		return EMPTY_HEAP;
	}

	element_offset = FindRemoveOffset(heap, element_offset, data, is_match,
																		param);
	if (0 == element_offset)
	{
		HeapPop(heap);

		return SUCCESS;
	}

	last_leaf_offset = FindLastLeaf(heap, last_leaf_offset);
	SwapData(&ELEMENT_DATA(element_offset), &ELEMENT_DATA(last_leaf_offset));
	ELEMENT_DATA(last_leaf_offset) = NULL;
	HeapifyDown(heap, last_leaf_offset, farther_child_offset);

	return SUCCESS;
}

void *HeapRemoveReturnData(heap_t *heap, const void *data,
									is_match_func_t is_match, const void *param)
{
	size_t element_offset = 0;
	size_t last_leaf_offset = 0;
	size_t farther_child_offset = 0;
	void *returned_data = NULL;

	assert(heap);
	assert(data);
	assert(is_match);

	if (0 == HeapSize(heap))
	{
		return NULL;
	}

	element_offset = FindRemoveOffset(heap, element_offset, data, is_match,
														param);
	returned_data = ELEMENT_DATA(element_offset);

	if (0 == element_offset)
	{
		HeapPop(heap);

		return NULL;
	}

	last_leaf_offset = FindLastLeaf(heap, last_leaf_offset);
	SwapData(&ELEMENT_DATA(element_offset), &ELEMENT_DATA(last_leaf_offset));
	ELEMENT_DATA(last_leaf_offset) = NULL;
	HeapifyDown(heap, last_leaf_offset, farther_child_offset);

	return returned_data;
}