/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdio.h> // fprintf - malloc error print
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
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
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
    for (int i = 0; i < HT_SIZE; i++) {
        (*table)[i] = NULL;
    }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
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
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
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
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
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
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
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
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po
 * inicializaci.
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
