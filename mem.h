#ifndef SCABS_MEM_H
#define SCABS_MEM_H

void *alloc_mem(size_t);
void free_mem(void*);
void set_mem(void *dest, int val, size_t s);
void cpy_mem(void *dest, void *src, size_t s);


#endif

