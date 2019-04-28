#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "cbuff.h"

#define MIN(a,b) (((a)<(b))? (a):(b))

struct cb_t
{
	size_t capacity;
	size_t read_offset;
	size_t write_offset;
	void  *base;
};

cb_t* CbuffCreate(size_t n_bytes)
{
	cb_t *cb = NULL;

	assert (n_bytes > 0);

	cb = (cb_t *)malloc(sizeof(cb_t) + n_bytes);
	if (NULL == cb)
	{
		return NULL;
	}

	cb->capacity = n_bytes;
	cb->read_offset = 0;
	cb->write_offset = 0;
	cb->base = (char *)cb + sizeof(cb_t);

	return cb;
}

size_t CbuffWrite(cb_t* cb, const void* input, size_t n_bytes)
{
	size_t first_copy = 0;
	
	assert (cb && input && n_bytes > 0);
	
	n_bytes = MIN(cb->capacity - cb->read_offset, n_bytes);
	first_copy = MIN(cb->capacity - cb->write_offset, n_bytes);

	memcpy((char*)cb->base + cb->write_offset, input, first_copy);
	memcpy((char*)cb->base, (char*)input + first_copy, n_bytes - first_copy);
	
	cb->write_offset += n_bytes;
	cb->write_offset %= cb->capacity;
	
	cb->read_offset += n_bytes;

	return n_bytes;

}

size_t CbuffRead(cb_t* cb, void* output, size_t n_bytes)
{
	size_t first_copy = 0;
	size_t read_position = 0;

	assert(cb && output && n_bytes > 0);
	
	read_position = (cb->write_offset + cb->capacity - cb->read_offset)
															 % cb->capacity;

	n_bytes = MIN(cb->read_offset, n_bytes);
	first_copy = MIN(n_bytes, cb->capacity - read_position);

	memcpy(output, (char*)cb->base + read_position, first_copy);
	memcpy((char*)output + first_copy, cb->base, n_bytes - first_copy);

	cb->read_offset -= n_bytes;

	return n_bytes;
}

size_t CbuffCapacity(const cb_t* cb)
{
	assert(cb);

	return cb->capacity;
}

size_t CbuffFreeSpace(const cb_t* cb)
{
	assert(cb);

	return cb->capacity - cb->read_offset;
}

void CbuffDestroy(cb_t* cb)
{
	assert(cb);

	free (cb);
}

int CbuffIsBuffEmpty(const cb_t* cb)
{
	assert(cb);
	
	return !cb->read_offset;
}