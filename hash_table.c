#include <assert.h>
#include <stdlib.h>

#include "hash_table.h"
#include "linked_list.h"

#define INDEXED_TABLE_SLOT(data) h_table->table[h_table->hash_func(data, h_table->param)]
#define PARAM h_table->param
#define SEARCH_SLIST_FOR_DATA SlistFind(INDEXED_TABLE_SLOT, \
											CompareData(data, &compare), PARAM)
#define IS_BUCKET_EMPTY(bucket) (NULL == bucket)
#define DATA_NOT_FOUND(data)  (NULL == data)
#define COMP_PARAM (compare_t *)param
#define TABLE_INDEX(index) h_table->table[index]
#define HASH h_table->hash_func

struct h_table_t
{
	size_t num_of_buckets;
	hash_func_t hash_func;
	slist_node_t **table;
	hash_cmp_func_t cmp_func;
	void *param;
};

typedef struct compare_t
{
	void *external_data;
	hash_cmp_func_t cmp_func;
	void *param;

} compare_t;

typedef struct info_t
{
	char *word;
	size_t count;
} info_t;

static int CompareData(const void *internal_data, const void *param)
{
	return ((COMP_PARAM)->cmp_func((COMP_PARAM)->external_data,
										internal_data, (COMP_PARAM)->param));
}

/*
* receives table size, hash table func, compare func and params.
* creates a hash table.
* returns the hash table if succeeded, NULL otherwise.
* O(1).
*/
h_table_t *HashCreate(const size_t table_size, hash_func_t hash_func, 
									hash_cmp_func_t cmp_func, void *const param)
{
	h_table_t *hash = NULL;

	assert(0 < table_size);
	assert(NULL != hash_func);
	assert(NULL != cmp_func);

	hash = malloc(sizeof(h_table_t));
	if (NULL == hash)
	{
		return NULL;
	}

	hash->num_of_buckets = table_size;
	hash->hash_func = hash_func;
	hash->cmp_func = cmp_func;
	hash->param = param;
	hash->table = calloc(hash->num_of_buckets, sizeof(slist_node_t));
	if (NULL == hash->table)
	{
		free(hash);
		
		return NULL;
	}

	return hash;
}

/*
* receives a hash table and destroys it.
* O(n)
*/
void HashDestroy(h_table_t *h_table)
{
	size_t i = 0;

	assert(h_table);

	while (i < h_table->num_of_buckets)
	{
		if (NULL != TABLE_INDEX(i))
		{
			SlistFreeAll(h_table->table[i]);
		}

		++i;
	}

	free(h_table->table);
	free(h_table);
}

/*
* receives a hash table and a data to insert.
* inserts the data to the hash table.
* returns status. 
* NOTE: no duplicated keys
* O(1) for average case. O(n) for worst case. 
*/
int HashInsert(h_table_t *h_table, void *const data)
{
	slist_node_t *existing_node = NULL, *new_node = NULL;
	unsigned int index = 0;
	compare_t compare = {0};

	assert(NULL != h_table);
	assert(NULL != data);

	compare.cmp_func = h_table->cmp_func;
	compare.external_data = (void *)data;

	index = h_table->hash_func(data, h_table->param);

	if (NULL == TABLE_INDEX(index))
	{
		TABLE_INDEX(index) = SlistCreateNode(data, NULL);
		if (NULL == TABLE_INDEX(index))
		{
			return MALLOC_FAIL;
		}
	}
	else
	{
		existing_node = SlistFind(TABLE_INDEX(index), CompareData, &compare);
		if (NULL == existing_node)
		{
			new_node = SlistCreateNode(data, NULL);
			if (NULL == new_node)
			{
				return MALLOC_FAIL;
			}
			SlistInsertAfter(TABLE_INDEX(index), new_node);
		}
		else
		{
			return DATA_EXISTS;
		}
	}

	return SUCCESS;
}


/*
* receives a hash table and a key.
* removes the data matches the key.
* returns the data removed.
* O(n).
*/
void *HashRemove(h_table_t *h_table, const void *const key)
{
	slist_node_t *before_node = NULL;
	void *returned_data = NULL;

	assert(NULL != h_table);
	assert(NULL != key);

	before_node = TABLE_INDEX(HASH((void *)key, h_table->param));
	if (NULL == before_node)
	{
		return NULL;
	}
	
	if (0 == h_table->cmp_func(before_node->data, key, PARAM))
	{
		if (NULL == before_node->next)
		{
			returned_data = before_node->data;
			free(before_node);
			TABLE_INDEX(HASH((void *)key, h_table->param)) = NULL;

			return returned_data;
		}
		else
		{
			returned_data = before_node->data;
			SlistRemove(before_node);

			return returned_data;
		}
	}

	while (NULL != before_node->next)
	{
		if (0 == h_table->cmp_func(before_node->next->data, key, PARAM))
		{
			returned_data = before_node->next->data;
			SlistRemoveAfter(before_node);
			return returned_data;
		}
		
		before_node = before_node->next;
	}

	return returned_data;
}

/*
* receives a hash table.
* Counts the number of elements in the hash table (O(n))
*/
size_t HashSize(const h_table_t *h_table)
{
	size_t i = 0;
	size_t counter = 0;

	assert(h_table);

	for (i = 0; i < h_table->num_of_buckets; ++i)
	{
		if (NULL != TABLE_INDEX(i))
		{
			counter += SlistCount(h_table->table[i]);
		}
	}

	return counter;
}

/*
* receives a hash table.
* returns a boolean value: 1 if the table is empty, 0 otherwise.
* O(n).
*/
int HashIsEmpty(const h_table_t *h_table)
{
	size_t i = 0;

	assert(h_table);

	for (i = 0; i < h_table->num_of_buckets; ++i)
	{
		if (NULL != TABLE_INDEX(i))
		{
			return 0;
		}
	}

	return 1;
}

/*
* receives a hash table and a key to find in the table.
* returns the data matches the key.
* O(n).
*/
void *HashFind(h_table_t *h_table, const void *const key)
{
	compare_t compare = {0};
	slist_node_t *node = NULL;

	assert(NULL != h_table);
	assert(NULL != key);

	compare.cmp_func = h_table->cmp_func;
	compare.external_data = (void *)key;

	node = TABLE_INDEX(HASH(compare.external_data, h_table->param));
	if (NULL == node)
	{
		return NULL;
	}
	
	node = SlistFind(node, CompareData, &compare);
	if (NULL != node)
	{
		return node->data;
	}
	
	return NULL;
}

/* 
* receives a hash table and an action function.
* Runs the function on all buckets in the hash table 
* O(n)
*/
int HashForEach(h_table_t *h_table, act_func_t action, void *param)
{
	size_t i = 0;
	int res = 0;

	assert(h_table);
	assert(action);

	for (i = 0; i < h_table->num_of_buckets; ++i)
	{
		if (NULL != TABLE_INDEX(i))
		{
			res = SlistForEach(TABLE_INDEX(i), action, param);
			if (0 != res)
			{
				return res;
			}
		}
	}

	return res;
}


