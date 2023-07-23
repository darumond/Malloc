#include "allo.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
void *page_begin(void *ptr, size_t page_size)
{
    char *res = ptr;
    res = res - ((size_t)ptr & (page_size - 1));
    ptr = res;
    return ptr;
}
int beware_overflow(size_t nmemb, size_t size)
{
    size_t tmp;
    if (__builtin_mul_overflow(nmemb, size, &tmp))
        return 1;
    return 0;
}
char *init_free(size_t size_free)
{
    char *tmp = mmap(NULL, size_free, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    for (size_t i = 0; i < size_free; i++)
        tmp[i] = '0';
    tmp[size_free] = '\0';
    char *res = tmp;
    return res;
}
struct bucket *bucket_init(size_t size)
{
    size_t len = sysconf(_SC_PAGESIZE);
    int bool_taille = 1;
    size_t bonus = 4 * sizeof(struct bucket *);
    if (len <= size)
    {
        len = size;
        bool_taille = 0;
    }
    struct bucket *new = mmap(NULL, len, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new == MAP_FAILED)
        return NULL;
    new->next = NULL;
    new->size = size;
    size_t size_free;
    if (bool_taille)
        size_free = (len - bonus) / size;
    else
        size_free = 1;
    new->free = init_free(size_free);
    return new;
}
size_t align(size_t size)
{
    size_t res;
    size_t mod = size % sizeof(long double);
    if (mod == 0)
    {
        return size;
    }
    if (__builtin_mul_overflow(sizeof(long double), size, &res))
    {
        return 0;
    }

    return size + sizeof(long double) - mod;
}
size_t round_power(size_t a)
{
    size_t res = 1;
    while (a > res)
        res *= 2;
    return res;
}
