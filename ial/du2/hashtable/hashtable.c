/*
 * Tabuľka s rozptýlenými položkami
 *
 * S využitím dátových typov zo súboru hashtable.h a pripravených kostier
 * funkcií implementujte tabuľku s rozptýlenými položkami s explicitne
 * zreťazenými synonymami.
 *
 * Pri implementácii uvažujte veľkosť tabuľky HT_SIZE.
 */

#include "hashtable.h"
#include <stdio.h> // kvuli vypisu chybove hlasky pri chybe mallocu
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptyľovacia funkcia ktorá pridelí zadanému kľúču index z intervalu
 * <0,HT_SIZE-1>. Ideálna rozptyľovacia funkcia by mala rozprestrieť kľúče
 * rovnomerne po všetkých indexoch. Zamyslite sa nad kvalitou zvolenej funkcie.
 */
int get_hash(char *key) {
    int result = 1;
    int length = strlen(key);
    for (int i = 0; i < length; i++) {
        result += key[i];
    }
    return (result % HT_SIZE);
}

/*
 * Inicializácia tabuľky — zavolá sa pred prvým použitím tabuľky.
 */
void ht_init(ht_table_t *table) {
    for (int i = 0; i < HT_SIZE; i++) {
        (*table)[i] = NULL;
    }
}

/*
 * Vyhľadanie prvku v tabuľke.
 *
 * V prípade úspechu vráti ukazovateľ na nájdený prvok; v opačnom prípade vráti
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
    if (table != NULL) {
        int pos = get_hash(key);
        if ((*table)[pos] != NULL) {
            if (*((*table)[pos]->key) == *key) {
                return (*table)[pos];
            } else {
                ht_item_t *tmp = (*table)[pos]->next;
                while (tmp != NULL) {
                    if (*tmp->key == *key) {
                        return tmp;
                    } else {
                        tmp = tmp->next;
                    }
                }
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    return NULL;
}

/*
 * Vloženie nového prvku do tabuľky.
 *
 * Pokiaľ prvok s daným kľúčom už v tabuľke existuje, nahraďte jeho hodnotu.
 *
 * Pri implementácii využite funkciu ht_search. Pri vkladaní prvku do zoznamu
 * synonym zvoľte najefektívnejšiu možnosť a vložte prvok na začiatok zoznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
    ht_item_t *tmp = ht_search(table, key);
    if (tmp != NULL) {
        tmp->value = value;
        return;
    }
    int pos = get_hash(key);
    if ((*table)[pos] == NULL) {
        (*table)[pos] = malloc(sizeof(ht_item_t));
        if ((*table)[pos] == NULL) {
            fprintf(stderr, "Chyba alokace pameti! \n");
            return;
        }
        (*table)[pos]->key = key;
        (*table)[pos]->value = value;
        (*table)[pos]->next = NULL;
    } else {
        tmp = (*table)[pos]; // search nic nenaslo - na pozici pos neni to, co hledame
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = malloc(sizeof(ht_item_t));
        if (tmp->next == NULL) {
            fprintf(stderr, "Chyba alokace pameti! \n");
            return;
        }
        tmp->next->key = key;
        tmp->next->next = NULL;
        tmp->next->value = value;
    }
}

/*
 * Získanie hodnoty z tabuľky.
 *
 * V prípade úspechu vráti funkcia ukazovateľ na hodnotu prvku, v opačnom
 * prípade hodnotu NULL.
 *
 * Pri implementácii využite funkciu ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
    ht_item_t *tmp = ht_search(table, key);
    if (tmp == NULL) {
        return NULL;
    } else {
        return &tmp->value;
    }
}

/*
 * Zmazanie prvku z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje priradené k danému prvku.
 * Pokiaľ prvok neexistuje, nerobte nič.
 *
 * Pri implementácii NEVYUŽÍVAJTE funkciu ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
    int pos = get_hash(key);
    if ((*table)[pos] != NULL) {
        if (*(*table)[pos]->key == *key) {
            if ((*table)[pos]->next == NULL) {
                free((*table)[pos]);
                (*table)[pos] = NULL;
            } else {
                ht_item_t *tmp = (*table)[pos]->next;
                free((*table)[pos]);
                (*table)[pos] = tmp;
            }
        } else {
            ht_item_t *tmp = (*table)[pos];
            while (tmp->next != NULL) {
                if (*tmp->next->key == *key) {
                    ht_item_t *ptr = tmp->next->next;
                    free(tmp->next);
                    tmp->next = ptr;
                    return;
                }
                tmp = tmp->next;
            }
        }
    }
}

/*
 * Zmazanie všetkých prvkov z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje a uvedie tabuľku do stavu po
 * inicializácii.
 */
void ht_delete_all(ht_table_t *table) {
    for (int i = 0; i < HT_SIZE; i++) {
        if ((*table)[i] != NULL) {
            ht_item_t *ptr = (*table)[i];
            ht_item_t *tmp;
            while (ptr->next != NULL) {
                tmp = ptr->next;
                free(ptr);
                ptr = tmp;
            }
            free(ptr);
            (*table)[i] = NULL;
        }
    }
}
