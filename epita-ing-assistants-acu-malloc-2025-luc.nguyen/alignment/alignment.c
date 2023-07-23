#include <stdio.h>
#include <stdlib.h>
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
