#include "allocator.h"

#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
size_t align(size_t size)
{
    size_t res;
    size_t mod = size % sysconf(_SC_PAGE_SIZE);
    if (mod == 0)
    {
        return size;
    }
    if (__builtin_mul_overflow(sysconf(_SC_PAGE_SIZE), size, &res))
    {
        return 0;
    }

    return size + sysconf(_SC_PAGE_SIZE) - mod;
}
struct blk_allocator *blka_new(void)
{
    struct blk_allocator *new = calloc(1, sizeof(struct blk_allocator));
    return new;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    size_t len = sizeof(struct blk_meta) + size;
    if (!blka)
        return NULL;
    struct blk_meta *page = mmap(NULL, align(len), PROT_READ | PROT_WRITE,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED)
        return NULL;
    page->size = align(len) - sizeof(struct blk_meta);
    page->next = blka->meta;
    blka->meta = page;
    return page;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, sizeof(struct blk_meta));
}

void blka_pop(struct blk_allocator *blka)
{
    if (!blka->meta)
        return;
    struct blk_meta *tmp = blka->meta->next;
    struct blk_meta *s = blka->meta;
    blka->meta = tmp;
    munmap(s, sizeof(struct blk_meta));
}

void blka_delete(struct blk_allocator *blka)
{
    if (!blka)
        return;
    while (blka->meta)
        blka_pop(blka);
    free(blka);
}
