#ifndef FINITE_MEM_H
#define FINITE_MEM_H

void *memAlloc(size_t);
void memFree(void*);
void memSet(void *dest, int val, size_t s);
void memCpy(void *dest, void *src, size_t s);


#endif

