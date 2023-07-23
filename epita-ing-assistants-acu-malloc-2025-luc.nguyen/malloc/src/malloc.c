#include <stddef.h>
#include <stdio.h>

#include "allo.h"

static struct bucket *big = NULL;
__attribute__((visibility("default"))) void *malloc(size_t size)
{
    size = round_power(size);
    struct bucket *tmp = big;
    struct bucket *small = big;
    if (!tmp)
    {
        big = bucket_init(size);
        tmp = big;
    }
    else
    {
        while (tmp)
        {
            if (tmp->size == size)
                break;
            small = tmp;
            tmp = tmp->next;
        }
        if (!tmp)
        {
            big = bucket_init(size);
            tmp = small->next;
        }
    }
    struct bucket *b = big;
    size_t i = 0;
    int isfull = 1;
    while (isfull)
    {
        i = 0;
        while (b->free[i] != '\0')
        {
            if (b->free[i] == '0')
            {
                b->free[i] = '1';
                isfull = 0;
                break;
            }
            i++;
        }
        if (isfull)
        {
            while (b->next)
            {
                b = b->next;
            }
            big = bucket_init(size);
            b->next = big;
        }
    }
    size_t add = 4 * sizeof(struct bucket *) + i * size;
    void *ptr = (void *)((char *)b + add);
    return ptr;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    if (!ptr)
        return;
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    int i = 0;
    if (!ptr)
    {
        return malloc(size);
    }
    struct bucket *page_b = page_begin(ptr, 4096);

    if (page_b->size == size)
    {
        return ptr;
    }
    char *page_b2 = (char *)page_b;
    char *len = page_b2 + sizeof(len) * 4;
    while (ptr != len)
    {
        len += page_b->size;
        i++;
    }
    if (page_b->free[i] == '0')
    {
        return NULL;
    }
    void *new = calloc(size, 1);
    if (page_b->size > size)
    {
        new = memcpy(new, ptr, size);
    }
    else
    {
        new = memcpy(new, ptr, page_b->size);
    }
    free(ptr); // marche si free de l'ordi
    return new;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    if (beware_overflow(nmemb, size))
        return NULL;
    char *ptr = malloc(size * nmemb);
    if (!ptr)
    {
        return NULL;
    }
    size_t i = 0;
    while (i < size * nmemb)
    {
        ptr[i] = 0;
        i++;
    }
    return (void *)ptr;
}
