#ifndef ALLO_H
#define ALLO_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

struct bucket
{
    size_t size;
    struct bucket *next;
    char *free;
};
size_t round_power(size_t a);
struct bucket *bucket_init(size_t size);
void *page_begin(void *ptr, size_t page_size);
int beware_overflow(size_t nmemb, size_t size);

#endif /* !ALLO_H */
