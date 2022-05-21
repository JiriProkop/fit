// ppm.c
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#include "ppm.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct ppm* ppm_read(const char * filename)
{
    FILE *soubor;
    soubor = fopen(filename, "r");
    if (soubor == NULL)
    {
        warning_msg("chyba při otevření souboru: %s\n", filename);
        return NULL;
    }
    char typ_souboru[3];
    unsigned xsize;
    unsigned ysize;
    int max_barva;

    //načítání hlavičky
    if (fscanf(soubor, "%s %u %u %d\n", typ_souboru, &xsize, &ysize, &max_barva) != 4)
    {
        warning_msg("chyba při čtení hlavičky souboru: %s\n", filename);
        goto chyba;
    }

    //kontrola formátu ppm
    if (typ_souboru[0] != 'P' || typ_souboru[1] != '6' || max_barva > 255 || max_barva < 0)
    {
        warning_msg("chybný formát souboru: %s\n", filename);
        goto chyba;
    }

    //kontrola velikosti souboru
    if (3 * sizeof(unsigned) * xsize * ysize > PPM_LIMIT_VELIKOSTI)
    {
        warning_msg("příliž velký soubor: %s\n", filename);
        goto chyba;
    }
    
    //vytvoření struct ppm* a nahrání dat
    struct ppm* obrazek = malloc(2 *sizeof(unsigned) + 3 * sizeof(unsigned) * xsize * ysize);
    if (obrazek == NULL)
    {
        warning_msg("Chyba alokace.\n");
        goto chyba;
    }
    obrazek->xsize = xsize;
    obrazek->ysize = ysize;

    if (fread(obrazek->data, sizeof(char), 3 * xsize * ysize, soubor) != (3 * xsize * ysize))
    {
        free(obrazek);
        warning_msg("Chyba při čtení souboru %s.\n", filename);
        goto chyba;
    }
    fclose(soubor);
    return obrazek;

    chyba:
        fclose(soubor);
        return NULL;
}

void ppm_free(struct ppm *p)
{
    free(p);
}