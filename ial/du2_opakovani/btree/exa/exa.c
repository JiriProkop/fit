/*
 * Použití binárních vyhledávacích stromů.
 *
 * S využitím Vámi implementovaného binárního vyhledávacího stromu (soubory ../iter/btree.c a ../rec/btree.c)
 * implementujte funkci letter_count. Výstupní strom může být značně degradovaný (až na úroveň lineárního seznamu)
 * a tedy implementujte i druhou funkci (bst_balance), která strom, na požadavek uživatele, vybalancuje.
 * Funkce jsou na sobě nezávislé a tedy automaticky NEVOLEJTE bst_balance v letter_count.
 *
 */

#include "../btree.h"
#include "../test_util.h"
#include <stdio.h>
#include <stdlib.h>

char get_relevant_char(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    } else if ((c >= 'a' && c <= 'z') || c == ' ') {
        return c;
    } else {
        return '_';
    }
}

/**
 * Vypočítání frekvence výskytů znaků ve vstupním řetězci.
 *
 * Funkce inicilializuje strom a následně zjistí počet výskytů znaků a-z (case insensitive), znaku
 * mezery ' ', a ostatních znaků (ve stromu reprezentováno znakem podtržítka '_'). Výstup je v
 * uložen ve stromu.
 *
 * Například pro vstupní řetězec: "abBccc_ 123 *" bude strom po běhu funkce obsahovat:
 *
 * key | value
 * 'a'     1
 * 'b'     2
 * 'c'     3
 * ' '     2
 * '_'     5
 *
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce.
 */
void letter_count(bst_node_t **tree, char *input) {
    bst_init(tree);
    int tmp;
    for (int i = 0; input[i] != '\0'; i++) {
        char c = get_relevant_char(input[i]);
        tmp = bst_search(*tree, c, &tmp) ? tmp + 1 : 1;
        bst_insert(tree, c, tmp);
    }
}

void insert_nodes(bst_node_t **tree, bst_items_t *items, int start, int end) {
    if (start > end) {
        return;
    }

    int mid = (start + end) / 2;
    int value = items->nodes[mid]->value;
    int key = items->nodes[mid]->key;
    bst_insert(tree, key, value);

    insert_nodes(tree, items, start, mid - 1);
    insert_nodes(tree, items, mid + 1, end);
}

/**
 * Vyvážení stromu.
 *
 * Vyvážený binární vyhledávací strom je takový binární strom, kde hloubka podstromů libovolného uzlu se od sebe liší maximálně o jedna.
 *
 * Předpokládejte, že strom je alespoň inicializován. K získání uzlů stromu využijte vhodnou verzi vámi naimplmentovaného průchodu stromem.
 * Následně můžete například vytvořit nový strom, kde pořadím vkládaných prvků zajistíte vyváženost.
 *
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce. Není nutné, aby funkce fungovala *in situ* (in-place).
 */
void bst_balance(bst_node_t **tree) {
    bst_items_t *items = bst_init_items();
    bst_inorder(*tree, items);

    bst_node_t *new_tree;
    bst_init(&new_tree);

    insert_nodes(&new_tree, items, 0, items->size - 1);

    bst_dispose(tree);
    *tree = new_tree;

    free(items->nodes);
    free(items);
}
