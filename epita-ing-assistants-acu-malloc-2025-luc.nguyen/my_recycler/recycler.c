#include "recycler.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size == 0 || total_size == 0)
    {
        return NULL;
    }
    if (total_size % block_size != 0)
    {
        return NULL;
    }
    if (block_size % sizeof(size_t) != 0)
    {
        return NULL;
    }
    struct recycler *new = malloc(sizeof(struct recycler));
    if (!new)
    {
        return NULL;
    }
    void *chunk = malloc(total_size);
    if (!chunk)
    {
        return NULL;
    }
    size_t cap = total_size / block_size;

    new->chunk = chunk;
    char *tmp = chunk;
    struct free_list *freeL = chunk;
    new->free = freeL;
    for (size_t i = 1; i < cap; i++)
    {
        chunk = tmp + i * block_size;
        freeL->next = chunk;
        freeL = freeL->next;
    }
    freeL->next = NULL;
    new->capacity = cap;
    new->block_size = block_size;

    return new;
}

void recycler_destroy(struct recycler *r)
{
    if (r)
    {
        free(r->chunk);
        free(r);
    }
}
void *recycler_allocate(struct recycler *r)
{
    if (!r)
    {
        return NULL;
    }
    struct free_list *res = r->free;
    if (!res)
    {
        return NULL;
    }
    struct free_list *tmp = res;
    r->free = tmp->next;
    return res;
}
void recycler_free(struct recycler *r, void *block)
{
    if (r && block)
    {
        struct free_list *h = r->free;
        r->free = block;
        struct free_list *free = r->free;
        free->next = h;
    }
}
