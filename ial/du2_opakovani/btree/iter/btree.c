/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h
 * a připravených koster funkcí implementujte binární vyhledávací
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
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
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    if (tree == NULL) {
        return false;
    }
    while (tree != NULL) {
        if (key < tree->key) {
            tree = tree->left;
        }

        else if (key == tree->key) {
            *value = tree->value;
            return true;
        } else {
            tree = tree->right;
        }
    }
    return false;
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
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    if (*tree == NULL) {
        *tree = malloc(sizeof(bst_node_t));
        if (*tree == NULL) {
            fprintf(stderr, "Chyba alokace pameti! '\n'");
            return;
        }
        (*tree)->key = key;
        (*tree)->value = value;
        (*tree)->left = NULL;
        (*tree)->right = NULL;
        return;
    }
    bst_node_t *tmp = *tree;
    while (1) {
        if (key < tmp->key) {
            if (tmp->left != NULL) {
                tmp = tmp->left;
            } else {
                tmp->left = malloc(sizeof(bst_node_t));
                if (tmp->left == NULL) {
                    fprintf(stderr, "Chyba alokace pameti! '\n'");
                    return;
                }
                tmp->left->key = key;
                tmp->left->value = value;
                tmp->left->left = NULL;
                tmp->left->right = NULL;
                return;
            }
        } else if (key == tmp->key) {
            tmp->value = value;
            return;
        } else {
            if (tmp->right != NULL) {
                tmp = tmp->right;
            } else {
                tmp->right = malloc(sizeof(bst_node_t));
                if (tmp->right == NULL) {
                    fprintf(stderr, "Chyba alokace pameti! '\n'");
                    return;
                }
                tmp->right->key = key;
                tmp->right->value = value;
                tmp->right->left = NULL;
                tmp->right->right = NULL;
                return;
            }
        }
    }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    if (*tree == NULL) {
        return;
    }
    bst_node_t *tmp = *tree;
    if ((*tree)->right == NULL) {
        target->key = (*tree)->key;
        target->value = (*tree)->value;
        if ((*tree)->left == NULL) {
            free((*tree)); // Nastavit na NULL v bst_delete
        } else {
            tmp = (*tree)->left;
            free(*tree);
            *tree = tmp;
        }
        return;
    }
    while (tmp->right->right != NULL) {
        tmp = tmp->right;
    }
    target->key = tmp->right->key;
    target->value = tmp->right->value;
    if (tmp->right->left != NULL) {
        bst_node_t *ptr = tmp->right->left;
        free(tmp->right);
        tmp->right = ptr;
    } else {
        free(tmp->right);
        tmp->right = NULL;
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
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {
    if (*tree == NULL) {
        return;
    }
    bst_node_t *tmp = *tree;
    while (1) {
        if (key < tmp->key) {
            if (tmp->left != NULL) {
                if (tmp->left->left == NULL && tmp->left->right == NULL) {
                    if (tmp->left->key == key) {
                        free(tmp->left);
                        tmp->left = NULL;
                    }
                    return;
                } else {
                    tmp = tmp->left;
                }
            } else {
                return;
            }
        } else if (key == tmp->key) {
            if (tmp->left == NULL && tmp->right == NULL) {
                free(tmp);
                *tree = NULL; // podminka splena jenom kdyz je root ve stroum sam
            } else if (tmp->left == NULL) {
                tmp->key = tmp->right->key;
                tmp->left = tmp->right->left;
                tmp->value = tmp->right->value;
                bst_node_t *ptr = tmp->right;
                tmp->right = tmp->right->right;
                free(ptr);
            } else if (tmp->right == NULL) {
                tmp->key = tmp->left->key;
                tmp->right = tmp->left->right;
                tmp->value = tmp->left->value;
                bst_node_t *ptr = tmp->left;
                tmp->left = tmp->left->left;
                free(ptr);
            } else {
                if (tmp->left->left == NULL && tmp->left->right == NULL) {
                    bst_replace_by_rightmost(tmp, &(tmp->left));
                    tmp->left = NULL;
                } else {
                    bst_replace_by_rightmost(tmp, &(tmp->left));
                }
            }
            return;
        } else {
            if (tmp->right != NULL) {
                if (tmp->right->left == NULL && tmp->right->right == NULL) {
                    if (tmp->right->key == key) {
                        free(tmp->right);
                        tmp->right = NULL;
                    }
                    return;
                } else {
                    tmp = tmp->right;
                }
            } else {
                return;
            }
        }
    }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {
    while (*tree != NULL) {
        bst_node_t *tmp = *tree;
        while (1) {
            if (tmp->right != NULL) {
                if (tmp->right->right != NULL) {
                    tmp = tmp->right;
                } else {
                    if (tmp->right->left == NULL) {
                        free(tmp->right);
                        tmp->right = NULL;
                        break;
                    } else {
                        tmp = tmp->right;
                    }
                }
            }
            if (tmp->left != NULL) {
                if (tmp->left->left != NULL) {
                    tmp = tmp->left;
                } else {
                    if (tmp->left->right == NULL) {
                        free(tmp->left);
                        tmp->left = NULL;
                        break;
                    } else {
                        tmp = tmp->left;
                    }
                }
            }
            if (tmp->left == NULL && tmp->right == NULL) {
                free(tmp); // melo by se stat jenom na konci u uvolneni korene
                *tree = NULL;
                break;
            }
        }
    }
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items) {
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        bst_add_node_to_items(tree, items);
        tree = tree->left;
    }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {
    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_leftmost_preorder(tree, &stack, items);

    while (!stack_bst_empty(&stack)) {
        tree = stack_bst_top(&stack);
        stack_bst_pop(&stack);
        bst_leftmost_preorder(tree->right, &stack, items);
    }
}

/*
 * Pomocná funkce pro iterativní inorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        tree = tree->left;
    }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {
    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_leftmost_inorder(tree, &stack);

    while (!stack_bst_empty(&stack)) {
        tree = stack_bst_top(&stack);
        stack_bst_pop(&stack);
        bst_add_node_to_items(tree, items);
        bst_leftmost_inorder(tree->right, &stack);
    }
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        stack_bool_push(first_visit, true);
        tree = tree->left;
    }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {
    bool froml;
    stack_bool_t bool_stack;
    stack_bool_init(&bool_stack);
    stack_bst_t node_stack;
    stack_bst_init(&node_stack);

    bst_leftmost_postorder(tree, &node_stack, &bool_stack);
    while (!stack_bst_empty(&node_stack)) {
        tree = stack_bst_top(&node_stack);
        froml = stack_bool_top(&bool_stack);
        stack_bool_pop(&bool_stack);
        if (froml) {
            stack_bool_push(&bool_stack, false);
            bst_leftmost_postorder(tree->right, &node_stack, &bool_stack);
        } else {
            stack_bst_pop(&node_stack);
            bst_add_node_to_items(tree, items);
        }
    }
}
