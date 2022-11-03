/*
 * Binárny vyhľadávací strom — rekurzívna varianta
 *
 * S využitím dátových typov zo súboru btree.h a pripravených kostier funkcií
 * implementujte binárny vyhľadávací strom pomocou rekurzie.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
    (*tree) = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
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
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
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
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    if ((*tree)->right == NULL) { // nejpravejsim nodem je root
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
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte rekurzívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
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
            bst_replace_by_rightmost((*tree), &(*tree)->left);
        }
    } else {
        bst_delete(&((*tree)->right), key);
    }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
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
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
    if (tree == NULL) {
        return;
    }
    bst_print_node(tree);
    if (tree->left != NULL) {
        bst_preorder(tree->left);
    }
    if (tree->right != NULL) {
        bst_preorder(tree->right);
    }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) { // z leva do prava
    if (tree == NULL) {
        return;
    }
    if (tree->left != NULL) {
        bst_inorder(tree->left);
    }
    bst_print_node(tree);
    if (tree->right != NULL) {
        bst_inorder(tree->right);
    }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
    if (tree == NULL) {
        return;
    }
    if (tree->left != NULL) {
        bst_postorder(tree->left);
    }
    if (tree->right != NULL) {
        bst_postorder(tree->right);
    }
    bst_print_node(tree);
}
