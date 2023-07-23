#include <stddef.h>
void *page_begin(void *ptr, size_t page_size)
{
    char *res = ptr;
    res = res - ((size_t)ptr & (page_size - 1));
    ptr = res;
    return ptr;
}
