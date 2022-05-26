// https://stackoverflow.com/questions/33791310/c-mmap-initialize-at-0xffffffff
// https://stackoverflow.com/questions/8475609/implementing-your-own-malloc-free-with-mmap-and-munmap
#include "memory.hpp"
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

void *malloc(unsigned int size)
{
    unsigned int *plen;
    unsigned int len = size + sizeof(size); // Add sizeof( size ) for holding length.

    plen = (unsigned int *)mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (plen == MAP_FAILED)
    {
        printf("map failed\n");
        return NULL;
    }

    *plen = len;               // First 4 bytes contain length.
    return (void *)(&plen[1]); // Memory that is after length variable.
}

void *calloc(unsigned int size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    unsigned int *plen = (unsigned int *)ptr - 1; // Get length variable.
    munmap(plen, *plen);
}
