// bitset.c
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#include "bitset.h"


extern inline bitset_index_t index_pole(bitset_index_t index);
extern inline unsigned chyba_alokace();
#ifdef USE_INLINE
    extern inline void bitset_free(bitset_t jmeno_pole);
    extern inline unsigned long bitset_size(bitset_t jmeno_pole);
    extern inline void bitset_setbit(bitset_t jmeno_pole, bitset_index_t index, bool vyraz);
    extern inline bool bitset_getbit(bitset_t jmeno_pole, bitset_index_t index);
#endif