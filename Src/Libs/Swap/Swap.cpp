#include <stdlib.h>

#include "Swap.h"
#include "..\Logging\Logging.h"

int NormalSwap(void *a, void *b, size_t ElementSize)
{
    CHECK(a == nullptr, "a = nullptr", -1);
    CHECK(b == nullptr, "b = nullptr", -1);

    int i = 0;
    for(i = 0; i < ElementSize/sizeof(long long); i++)
    {
        long long c = ((long long*)a)[i];

        ((long long*)a)[i] = ((long long*)b)[i];
        ((long long*)b)[i] = c;
    }

    for(int j = i*sizeof(long long); j < ElementSize; j++)
    {
        char c = ((char*)a)[j];

        ((char*)a)[j] = ((char*)b)[j];
        ((char*)b)[j] = c;
    }

    return 0;
}