// ppm.h
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#ifndef PPM
#define PPM

#define PPM_LIMIT_VELIKOSTI (8000 * 8000 * 3)

#include <stdio.h>
#include <stdlib.h>

struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];    // RGB bajty, celkem 3*xsize*ysize
};

struct ppm* ppm_read(const char* filename);
// Při správném formátu nahraje informace ze souboru do struct ppm, 
// kterou zároveň alokuje.

void ppm_free(struct ppm* p);
// Uvolní alokovaný struct ppm.


#endif