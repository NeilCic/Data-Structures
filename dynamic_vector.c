#include "dynamic_vector.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define FACTOR 2
#define TESTFACTOR 1/4

#define STRUCTBASE dv->base
#define STRUCTNUMBER dv->number_of_elements
#define STRUCTELEMSIZE dv->size_of_element
#define STRUCTCAPA dv->capacity

struct dv_t
{
	size_t number_of_elements;
	size_t size_of_element;
	size_t capacity;
	void *base;
};


dv_t *DvCreate(size_t size_of_element, size_t num_of_elements)
{
	dv_t *dv = NULL;

	assert(size_of_element > 0 && num_of_elements > 0);
	
	dv = malloc(sizeof(dv_t));
	if (NULL == dv)
	{
		return NULL;
	}

	STRUCTBASE = malloc(size_of_element * num_of_elements);
	if (NULL == STRUCTBASE)
	{
		free(dv);
		
		return NULL;
	}

	STRUCTNUMBER = 0;
	STRUCTELEMSIZE = size_of_element;
	STRUCTCAPA = num_of_elements;

	return dv;
}

int DvPushBack(dv_t* dv, const void* element)
{
	assert(dv);

	if (STRUCTNUMBER == STRUCTCAPA)
	{
		STRUCTBASE = realloc(STRUCTBASE, FACTOR * STRUCTCAPA 
										* STRUCTELEMSIZE);
		if (NULL == STRUCTBASE)
	{
		return -1;
	}

		STRUCTCAPA *= FACTOR;
}

	memcpy((char*)STRUCTBASE + STRUCTELEMSIZE * STRUCTNUMBER, element, 
														STRUCTELEMSIZE);

	++STRUCTNUMBER;
					
	return 0;
}

int DvPopBack(dv_t* dv)
{
	assert(dv);

	--STRUCTNUMBER;

	if (STRUCTNUMBER <= STRUCTCAPA * TESTFACTOR)
	{
		STRUCTBASE = realloc(STRUCTBASE, STRUCTCAPA * STRUCTELEMSIZE / FACTOR);
		if (NULL == STRUCTBASE)
		{
			return -1;
		}

		STRUCTCAPA /= FACTOR;
	}

	return 0;
}

int DvReserve(dv_t* dv, size_t new_capacity)
{
	assert(dv);

	STRUCTBASE = realloc(STRUCTBASE, new_capacity);
	if (NULL == STRUCTBASE)
	{
		return -1;
	}

	STRUCTCAPA = new_capacity;

	return 0;
}

void* DvGetItemByIndex(dv_t* dv, size_t index_of_element)
{
	assert(dv);

	return ((char*)STRUCTBASE + index_of_element * STRUCTELEMSIZE);
} 

size_t DvSize(const dv_t* dv)
{
	assert(dv);

	return STRUCTNUMBER;
}

size_t DvCapacity(const dv_t* dv)
{
	assert(dv);

	return STRUCTCAPA;
}

void DvDestroy(dv_t* dv)
{
	assert(dv);

	free(STRUCTBASE);
	free(dv);
}
