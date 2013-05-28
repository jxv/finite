#include <memory.h>


#include "mem.h"
#include "common.h"


void set_mem(void *dest, int val, size_t s)
{
	NOT(dest), assert(s > 0);
	memset(dest, val, s);
}


void cpy_mem(void *dest, void *src, size_t s)
{
	NOT(dest), NOT(src), assert(s > 0);
	memcpy(dest, src, s);
}


void *alloc_mem(size_t s)
{
	void *m;
	assert(s > 0);
	m = malloc(s);
	NOT(m);
	return m;
}


void free_mem(void *m)
{
	NOT(m);
	free(m);
}


