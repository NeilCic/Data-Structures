#include <assert.h>

#include "btrie.h"

#define LSB(data) ((data) & 1)
#define ISLEAF(node) (NULL == (node)->child[0])? 1 : 0


enum return_status
{
	SUCCESS,
	MALLOC_FAIL,
	STATUS_FAIL
};

enum node_status
{
	FULL_OCCUPANCY_BELOW_NODE,
	VACANT_BELOW_NODE,
	PARTIAL_OCCUPANCY_BELOW_NODE
};

static size_t PowerOfTwo(size_t bit_size_limit)
{
	size_t res = 1;

	while (bit_size_limit > 0)
	{
		res *= 2;
		--bit_size_limit;
	}

	return res;
}

static void InitNode(btrie_node_t *node, int status)
{
	assert(node);

	node->status = status;
	node->child[0] = NULL;
	node->child[1] = NULL;
}

static int InitChildrenNodes(btrie_node_t *node)
{
	btrie_node_t *node1 = NULL;
	btrie_node_t *node2 = NULL;

	assert(node);

	node1 = malloc(sizeof(btrie_node_t));
	if (NULL == node1)
	{
		return MALLOC_FAIL;
	}
	
	node2 = malloc(sizeof(btrie_node_t));
	if (NULL == node2)
	{
		free(node1);
		return MALLOC_FAIL;
	}

	InitNode(node1, node->status);
	InitNode(node2, node->status);

	node->child[0] = node1;
	node->child[1] = node2;

	return SUCCESS;
}

static void FreeChildren(btrie_node_t *node)
{
	free(node->child[0]);
	free(node->child[1]);
	node->child[0] = NULL;
	node->child[1] = NULL;
}

btrie_t *BTrieCreate(size_t bit_size_limit)
{
	btrie_t *trie = NULL;

	assert(bit_size_limit > 0);

	trie = malloc(sizeof(btrie_t));
	if (NULL == trie)
	{
		return NULL;
	}

	trie->root = malloc(sizeof(btrie_node_t));
	if (NULL == trie->root)
	{
		free(trie);
		return NULL;
	}

	trie->bit_size_limit = bit_size_limit;
	trie->max_addresses = PowerOfTwo(bit_size_limit) - 1;
	trie->root->child[0] = NULL;
	trie->root->child[1] = NULL;
	trie->root->status = VACANT_BELOW_NODE;

	return trie;
}

static void BTrieDestroyNodesRec(btrie_node_t *node)
{
	assert(node);

	if (NULL != node->child[0])
	{
		BTrieDestroyNodesRec(node->child[0]);
	}

	if (NULL != node->child[1])
	{
		BTrieDestroyNodesRec(node->child[1]);
	}

	free(node);
	node = NULL;
}

void BTrieDestroy(btrie_t *trie)
{
	assert(trie);

	BTrieDestroyNodesRec(trie->root);

	free(trie);
	trie = NULL;
}

static void UpdateNodeStatus(btrie_node_t *node)
{
	if (!ISLEAF(node))
	{
		if ((FULL_OCCUPANCY_BELOW_NODE == node->child[0]->status) &&
			(FULL_OCCUPANCY_BELOW_NODE == node->child[1]->status))
		{
			node->status = FULL_OCCUPANCY_BELOW_NODE;
			FreeChildren(node);
		}
		else if ((VACANT_BELOW_NODE == node->child[0]->status) &&
			   (VACANT_BELOW_NODE == node->child[1]->status))
		{
			node->status = VACANT_BELOW_NODE;
			FreeChildren(node);
		}
		else
		{
			node->status = PARTIAL_OCCUPANCY_BELOW_NODE;
		}
	}
}

static int BTrieInsertOrRemoveRec(btrie_node_t *node, unsigned int data,
										size_t level_counter, int param)
{
	int res = 0;
	int child_number = 0;

	assert(NULL != node);

	if (param == node->status)
	{
		return STATUS_FAIL;
	}

	if (0 == level_counter)
	{
		node->status = param;

		return SUCCESS;
	}

	if (ISLEAF(node))
	{
		if (MALLOC_FAIL == InitChildrenNodes(node))
		{
			return MALLOC_FAIL;
		}
	}
	child_number = ((data >> (level_counter - 1))) & 1;
	res = BTrieInsertOrRemoveRec(node->child[child_number], data,
													level_counter - 1, param);

	UpdateNodeStatus(node);

	return res;
}

int BTrieInsert(btrie_t *trie, unsigned int data)
{
	assert(NULL != trie);

	data = (data << trie->bit_size_limit);
	data = (data >> trie->bit_size_limit);

	return BTrieInsertOrRemoveRec(trie->root, data, trie->bit_size_limit, 
													FULL_OCCUPANCY_BELOW_NODE);
}

int BTrieFreeNode(btrie_t *trie, unsigned int data)
{
	assert(NULL != trie);

	data = (data << trie->bit_size_limit);
	data = (data >> trie->bit_size_limit);
	
	return BTrieInsertOrRemoveRec(trie->root, data, trie->bit_size_limit, 
															VACANT_BELOW_NODE);
}

static unsigned int BTrieGetNewNodeRec(btrie_node_t *node, unsigned int data,
														size_t level_counter)
{
	if (0 == level_counter)
	{
		return data;
	}

	--level_counter;
	if (PARTIAL_OCCUPANCY_BELOW_NODE == node->child[0]->status)
	{
		return BTrieGetNewNodeRec(node->child[0], data, level_counter);
	}
	else if (PARTIAL_OCCUPANCY_BELOW_NODE == node->child[1]->status)
	{
		data |= (1<<level_counter);

		return BTrieGetNewNodeRec(node->child[1], data, level_counter);
	}

	if (VACANT_BELOW_NODE == node->child[1]->status)
	{
		data |= (1 << level_counter);
	}

	return data;
}

unsigned int BTrieGetNewNode(btrie_t *trie)
{
	unsigned int data = 0;

	assert(NULL != trie);

	if (FULL_OCCUPANCY_BELOW_NODE == trie->root->status)
	{
		return -1;
	}

	if (VACANT_BELOW_NODE == trie->root->status)
	{
		return 0;
	}

	return BTrieGetNewNodeRec(trie->root, data, trie->bit_size_limit);
}

static size_t NodeCountRec(btrie_node_t *node)
{
	size_t counter = 1;

	assert(NULL != node);

	if (NULL != node->child[0])
	{
		counter += NodeCountRec(node->child[0]);
	}

	if (NULL != node->child[1])
	{
		counter += NodeCountRec(node->child[1]);
	}

	return counter;
}

size_t BTrieCount(btrie_t *trie)
{
	assert(NULL != trie);

	return NodeCountRec(trie->root);
}

size_t BTrieMemoryConsumption(btrie_t *trie)
{
	assert(NULL != trie);

	return (BTrieCount(trie) * sizeof(btrie_node_t) + sizeof(btrie_t));
}

static size_t NodeCountVacantRec(btrie_node_t *node, size_t level_counter)
{
	size_t counter = 0;

	assert(NULL != node);

	if (VACANT_BELOW_NODE == node->status)
	{
		counter += PowerOfTwo(level_counter);

		return counter;
	}

	--level_counter;

	if (NULL != node->child[0])
	{
		counter += NodeCountVacantRec(node->child[0], level_counter);
	}

	if (NULL != node->child[1])
	{
		counter += NodeCountVacantRec(node->child[1], level_counter);
	}

	return counter;
}

size_t BTrieCountVacant(btrie_t *trie)
{
	assert(NULL != trie);

	return NodeCountVacantRec(trie->root, trie->bit_size_limit);
}