// eratosthenes.c
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#include <math.h>
#include "eratosthenes.h"
#include "bitset.h"

void Eratosthenes(bitset_t pole)
{
    bitset_index_t index = 2;
    bitset_index_t max = bitset_size(pole);
    bitset_index_t limit = sqrt(max);

    while(index < limit)
    {
        if (bitset_getbit(pole, index) == 0)
        {
            for (bitset_index_t i = index * index; i < max; i += index)
            {
                bitset_setbit(pole, i, 1);
            }
        }
        ++index;
    }
}