#include <memory.h>


#include "mem.h"
#include "common.h"


void memSet(void *dest, int val, size_t s)
{
	NOT(dest);
	assert(s > 0);

	memset(dest, val, s);
}


void memCpy(void *dest, void *src, size_t s)
{
	NOT(dest);
	NOT(src);
	assert(s > 0);

	memcpy(dest, src, s);
}


void *memAlloc(size_t s)
{
	void *m;

	assert(s > 0);

	m = malloc(s);
	NOT(m);

	return m;
}


void memFree(void *m)
{
	NOT(m);

	free(m);
}


