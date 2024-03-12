// @file: cjlib_dictionary.c
/**
 * This file contains an implementation of a dictionary data structure.
 * The implementation is using the Binary search tree's representation 
 * in order to achive the best perfomance of O(log n).
*/

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "cjlib_dictionary.h"
#include "cjlib_queue.h"

// Those constants are used in the search/insert/delete functions.
#define S_RETRIEVE_KEY_NODE        0x0
#define S_RETRIEVE_KEY_NODE_PARENT 0x1

// Those constatns are used in the lvl_order_traversal function, for the delete_nodes flag.
#define T_DONT_DELETE_NODES 0x0
#define T_DELETE_NODES      0x1

// Those macros are used to determine if the AVL is balanced.
#define T_TREE_BALANCED_HEIGH 0x1
#define T_TREE_IS_BALANCED(BALANCE_FACTOR) BALANCE_FACTOR <= T_TREE_BALANCED_HEIGH

/**
 * This function calculate the height of a tree/subtree using
 * the lvl order traversal method combined with a queue. Also, 
 * it free's the memory of each tree node, if delete_nodes flag is set
 * to true.
 * 
 * @param src The subtree of interest.
 * @param delete_nodes If this flag is true, then we free the space during the traverasal of the tree.
 * @returns The height of the subree.
*/
static size_t lvl_order_traversal(const struct avl_bs_tree_node *restrict src, bool delete_nodes)
{
    if (NULL == src || NULL == src->avl_left || NULL == src->avl_right) {
        return 0;
    }

    struct cjlib_queue lvl_traversal_q;
    cjlib_queue_init(&lvl_traversal_q);

    struct avl_bs_tree_node *tmp = NULL;
    size_t current_lvl = 0;

    if (0 != cjlib_queue_enquue(src, &lvl_traversal_q)) return -1;

    while (!cjlib_queue_is_empty(&lvl_traversal_q)) {
        if (delete_nodes) free(tmp); 
        // NO if - statement needed to ensure that left or right child is NULL,
        // cause the queue will reject any NULL source.
        for (int i = 0; i < cjlib_queue_size(&lvl_traversal_q); i++) {
            cjlib_queue_deqeue(tmp, &lvl_traversal_q);
            if (0 != cjlib_queue_enqeue(tmp->avl_left, &lvl_traversal_q)) return -1;   
            if (0 != cjlib_queue_enqeue(tmp->avl_right, &lvl_traversal_q)) return -1;
        }
        current_lvl += 1;
    }

    return current_lvl - 1;
}

/**
 * This function returns the height of a subtree.
 * @param src A pointer that ponits to the root of the subtree.
 * @returns The height of this subtree.
*/
static inline size_t get_node_height(const struct avl_bs_tree_node *restrict src) 
{
    return lvl_order_traversal(src, T_DONT_DELETE_NODES);
}

/**
 * This function calculate the balance factor of a binary search tree.
 * @param src A pointer that points to the AVL tree of interest
 * @return An integer that represent the ballance factor of the AVL tree.
*/
static inline int calc_balance_factor(const struct avl_bs_tree_node *restrict src)
{
    // Get the height of both children
    int left_subtree_h  = get_node_height(src->avl_left);
    int right_subtree_h = get_node_height(src->avl_right);

    // Calculate the balance factor.
    return (int) abs(left_subtree_h - right_subtree_h);
}

/**
 * This function searches for a node that is equal to the parameter @key
 * and if there is a node with such a key, it returns a pointer to the 
 * coresponded node in the AVL tree. If parameter @get_parent is true, then
 * instead of returning the node that is equal to the key, it returns the parent
 * of this node.
 * 
 * @param dict A pointer to the dictionary of interest.
 * @param key The key of interest.
 * @param get_parent If true, retrieves the parent of the node that owns the key of interest.
 * @returns A pointer to the AVL node that holds the key of interest, if get_parent is false. A pointer
 *          to the AVL node that is parent to the node that holds the key of interest if get_parent is true.
 *          Otherwise NULL.
 * 
*/
static struct avl_bs_tree_node *search_node(const struct avl_bs_tree_node *dict, const char *restrict key, 
                                            bool get_parent)
{
    struct avl_bs_tree_node *curr_node = dict;
    struct avl_bs_tree_node *curr_parent = dict;
    int compare_key = 0;
    while (curr_node) {
        compare_key = strcmp(key, curr_node->avl_key);
        if (compare_key > 0 || compare_key < 0) curr_parent = curr_node;

        if (compare_key > 0) {
            curr_node = curr_node->avl_right;
        } else if (compare_key < 0) {
            curr_node = curr_node->avl_left;
        } else {
            break;
        }
    }
    return (get_parent == false)? curr_node:curr_parent;
}

int cjlib_dict_search(struct cjlib_json_data *restrict dst, const struct avl_bs_tree_node *restrict dict, 
                      const char *restrict key)
{
    struct avl_bs_tree_node *tmp = search_node(dict, key, S_RETRIEVE_KEY_NODE);
    if (NULL == tmp) {
        // There is no node with such a key.
        return -1;
    } else {
        (void)memcpy(dst, tmp->avl_data, sizeof(struct cjlib_json_data));
    }

    return 0;
}

int cjlib_dict_insert(const struct cjlib_json_data *restrict src, struct avl_bs_tree_node *restrict dict,
                      const char *restrict key)
{
    struct cjlib_json_data tmp;
    struct avl_bs_tree_node *parent_node;
    struct avl_bs_tree_node *new_node;
    int compare;
    // A node with this key, already exists.
    if (-1 == cjlib_dict_search(&tmp, dict, key)) return -1;
    // Retrieve the node that will be the parent of node that holds the key.
    parent_node = search_node(dict, key, S_RETRIEVE_KEY_NODE_PARENT);

    // Make the new node.
    new_node = (struct avl_bs_tree_node *) malloc(sizeof(struct avl_bs_tree_node));
    cjlib_dict_init(new_node);

    new_node->avl_key = key;
    (void)memcpy(&new_node->avl_data, src, sizeof(struct cjlib_json_data));

    // Decide where to put the new node.
    compare = strcmp(key, parent_node->avl_key);
    if (compare > 0) {
        parent_node->avl_right = new_node;
    } else {
        parent_node->avl_left = new_node;
    }

    // AFTER INSERTION
    if (T_TREE_IS_BALANCED(calc_balance_factor(dict))) {
        // OK
    } else {
        // Not ok, the tree is not balanced.
    }
}

int cjlib_dict_remove(struct avl_bs_tree_node *dict, const char *restrict key)
{
    // AFTER DELETION
    if (T_TREE_IS_BALANCED(calc_balance_factor(dict))) {
        // OK
    } else {
        // Not ok, the tree is not balanced.
    }
}

size_t cjlib_dict_destroy(cjlib_dict *dict)
{
    return lvl_order_traversal(dict, T_DELETE_NODES);
}