/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree) {
    (*tree) = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    if (tree == NULL) {
        return false;
    } else if (key < tree->key) {
        return bst_search(tree->left, key, value);
    } else if (key == tree->key) {
        *value = tree->value;
        return true;
    } else { // key > tree->key
        return bst_search(tree->right, key, value);
    }
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    if (*tree == NULL) { // prazdny strom
        *tree = malloc(sizeof(bst_node_t));
        if (*tree == NULL) {
            fprintf(stderr, "Chyba alokace pameti! '\n'");
            return;
        }
        (*tree)->value = value;
        (*tree)->key = key;
        (*tree)->left = NULL;
        (*tree)->right = NULL;
    } else if (key < (*tree)->key) {
        bst_insert(&((*tree)->left), key, value);
    } else if (key == (*tree)->key) {
        (*tree)->value = value;
    } else {
        bst_insert(&((*tree)->right), key, value);
    }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    if ((*tree)->right == NULL) { // nejpravejsim nodem je root daneho podstromu
        target->key = (*tree)->key;
        target->value = (*tree)->value;
        if ((*tree)->left != NULL) {
            bst_node_t *tmp = (*tree)->left;
            free(*tree);
            *tree = tmp;
        } else {
            free(*tree);
        }
    } else {
        if ((*tree)->right->right != NULL) {
            bst_replace_by_rightmost(target, &((*tree)->right));
        } else {
            target->key = (*tree)->right->key;
            target->value = (*tree)->right->value;
            if ((*tree)->right->left != NULL) {
                bst_node_t *tmp = (*tree)->right->left;
                free((*tree)->right);
                (*tree)->right = tmp;
            } else {
                free((*tree)->right);
                (*tree)->right = NULL;
            }
        }
    }
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {
    if ((*tree) == NULL) {
        return;
    } else if ((*tree)->key > key) {
        bst_delete(&((*tree)->left), key);
    } else if ((*tree)->key == key) {
        if ((*tree)->left == NULL) {
            bst_node_t *tmp = (*tree)->right;
            free(*tree);
            *tree = tmp;
        } else if ((*tree)->right == NULL) {
            bst_node_t *tmp = (*tree)->left;
            free(*tree);
            *tree = tmp;
        } else {
            if ((*tree)->left->right == NULL && (*tree)->left->left == NULL) {
                bst_replace_by_rightmost((*tree), &(*tree)->left);
                (*tree)->left = NULL;
            } else {
                bst_replace_by_rightmost((*tree), &(*tree)->left);
            }
        }
    } else {
        bst_delete(&((*tree)->right), key);
    }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {
    if ((*tree) == NULL) {
        return;
    }
    if ((*tree)->right != NULL) {
        if ((*tree)->right->right != NULL || (*tree)->right->left != NULL) {
            bst_dispose(&((*tree)->right));
        } else {
            free((*tree)->right);
            (*tree)->right = NULL;
        }
    }
    if ((*tree)->left != NULL) {
        if ((*tree)->left->right != NULL || (*tree)->left->left != NULL) {
            bst_dispose(&((*tree)->left));
        } else {
            free((*tree)->left);
            (*tree)->left = NULL;
        }
    }
    if ((*tree)->left == NULL && (*tree)->right == NULL) {
        free(*tree);
        *tree = NULL;
    }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {
    if (tree == NULL) {
        return;
    }
    bst_add_node_to_items(tree, items);
    if (tree->left != NULL) {
        bst_preorder(tree->left, items);
    }
    if (tree->right != NULL) {
        bst_preorder(tree->right, items);
    }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {
    if (tree == NULL) {
        return;
    }
    if (tree->left != NULL) {
        bst_inorder(tree->left, items);
    }
    bst_add_node_to_items(tree, items);
    if (tree->right != NULL) {
        bst_inorder(tree->right, items);
    }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {
    if (tree == NULL) {
        return;
    }
    if (tree->left != NULL) {
        bst_postorder(tree->left, items);
    }
    if (tree->right != NULL) {
        bst_postorder(tree->right, items);
    }
    bst_add_node_to_items(tree, items);
}
