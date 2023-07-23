#include "beware_overflow.h"

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t tmp;
    if (__builtin_mul_overflow(nmemb, size, &tmp))
        return NULL;
    char *test = ptr;
    return test + tmp;
}
