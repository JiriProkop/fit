// steg-decode.c
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#include "ppm.h"
#include <stdio.h>
#include "error.h"
#include "bitset.h"
#include "eratosthenes.h"
#include "math.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error_exit("Nesprávný počet argumentů programu '%s'\n", argv[1]);
    }
    struct ppm* obrazek_ptr = ppm_read(argv[1]);

    const unsigned velikost_dat = (obrazek_ptr->xsize * obrazek_ptr->ysize * 3);
    bitset_alloc(prvocisla, velikost_dat);
    Eratosthenes(prvocisla);

    unsigned char Byte_zpravy[] = {0};
    unsigned char to_set;
    char counter = 0;
    for (bitset_index_t i = 29; i < 3 * obrazek_ptr->xsize * obrazek_ptr->ysize; i++)
    {
        if (bitset_getbit(prvocisla, i) != 0)
        {
            continue;
        }

        if (counter == CHAR_BIT)
        {
            if (Byte_zpravy[0] == '\0')
            {
                putchar('\n');
                bitset_free(prvocisla);
                ppm_free(obrazek_ptr);
                return 0;
            }
            putchar(Byte_zpravy[0]);
            counter = 0;
        }

        to_set = 1;
        to_set &= obrazek_ptr->data[i];
        if (to_set)
        {
            unsigned char bit = 1;
            bit <<= counter;
            Byte_zpravy[0] |= bit;
        }
        else
        {
            unsigned char bit = 1;
            bit <<= counter;
            bit = ~bit;
            Byte_zpravy[0] &= bit;
        }
        counter++;
    }    
    bitset_free(prvocisla);
    ppm_free(obrazek_ptr);
    error_exit("Zpráva v %s není korektně ukončena.\n", argv[1]);
    return -1;
}