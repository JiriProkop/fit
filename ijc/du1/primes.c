// primes.c
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0
// Pro lokální pole je třeba zvětšit limit velikosti zásobníku.

#include <time.h>
#include "eratosthenes.h"

#define BITSET_SIZE 300000000
#define BITSET_NAME q

int main()
{
    bitset_create(BITSET_NAME, BITSET_SIZE);
    
    clock_t start = clock();
    Eratosthenes(BITSET_NAME);
    fprintf(stderr, "Time=%.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC);

    unsigned long prvocisla[10];
    bitset_index_t index = bitset_size(BITSET_NAME) - 1;
    for (int i = 0; i < 10; index--)
    {
        if (bitset_getbit(BITSET_NAME, index) == 0)
        {
            prvocisla[i] = index;
            i++;
        }
    }
    for (int i = 9; i >= 0; i--)
    {
        printf("%lu\n", prvocisla[i]);
    }
    return 0;
}