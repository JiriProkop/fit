// bitset.h
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#ifndef bitset
#define bitset

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "error.h"

#define BITY_LONGU (CHAR_BIT * sizeof(unsigned long))
#define pocet_bytu_pole(jmeno_pole, velikost) (((velikost % BITY_LONGU != 0) ? \
                                                 velikost / BITY_LONGU + 1   : \
                                                 velikost / BITY_LONGU))
// je stejne jako:
// int pocetBytu = velikost / BITY_LONGU;
// if (velikost % BITY_LONGU)
// {
//     pocetBytu++;
// }

typedef unsigned long bitset_index_t;
typedef unsigned long* bitset_t;

#define bitset_create(jmeno_pole, velikost)                                            \
    unsigned long jmeno_pole[pocet_bytu_pole(jmeno_pole, velikost) + 1] = {(velikost)};\
    static_assert(velikost > 0, "Velikost bitoveho pole musi by kladne cislo!");
    

#define bitset_alloc(jmeno_pole, velikost)                                                         \
    bitset_t jmeno_pole = calloc(pocet_bytu_pole(jmeno_pole, velikost) + 1, sizeof(unsigned long));\
    jmeno_pole[0] = jmeno_pole != NULL ? velikost : chyba_alokace();                               \
    assert(!(velikost <= 0));

inline bitset_index_t index_pole(bitset_index_t index) {return 1 + index / BITY_LONGU;}
inline unsigned chyba_alokace()
{
    fprintf(stderr, "bitset_alloc: Chyba alokace paměti");
    exit(-1);
    return 0;
}
    //free, size, setbit, getbit
#ifdef USE_INLINE
    inline unsigned long bitset_size(bitset_t jmeno_pole) { return jmeno_pole[0]; }

    inline void bitset_free(bitset_t jmeno_pole)
    {
        free(jmeno_pole);
    }
    inline void bitset_setbit(bitset_t jmeno_pole, bitset_index_t index, bool vyraz)
    {
        bitset_index_t mez = jmeno_pole[0];
        if (index > mez)
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", index, mez);
        if (vyraz)
        {
            jmeno_pole[index_pole(index)] |= (unsigned long)1  << index % BITY_LONGU;
        }
        else
        {
            jmeno_pole[index_pole(index)] &= ~((unsigned long)1  << index % BITY_LONGU);
        }
    }

    inline bool bitset_getbit(bitset_t jmeno_pole, bitset_index_t index)
    {
        bitset_index_t mez = jmeno_pole[0];
        if (index > mez)
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", index, mez);
        return jmeno_pole[index_pole(index)] & ((unsigned long) 1 << (index % BITY_LONGU));
    }
#else
    #define bitset_size(jmeno_pole) (jmeno_pole[0])
    #define bitset_free(jmeno_pole) free(jmeno_pole)
    #define bitset_setbit(jmeno_pole, index, vyraz)                                                                     \
        if((unsigned long)index >= bitset_size(jmeno_pole))                                                             \
        error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long)index, (unsigned long)jmeno_pole[0]-1);\
        (bool) vyraz ? (jmeno_pole[index_pole((unsigned long)index)] |=  (1UL << (unsigned long)index % BITY_LONGU))    \
                     : (jmeno_pole[index_pole((unsigned long)index)] &= ~(1UL << (unsigned long)index % BITY_LONGU))

    #define bitset_getbit(jmeno_pole, index) (jmeno_pole[index_pole((unsigned long)index)] & (1UL << ((unsigned long)index % BITY_LONGU)) )  

#endif //ifdef USE_INLINE
#endif