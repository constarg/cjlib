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


/**
 * This function calculate the height of a tree/subtree using
 * the lvl order traversal method combined with a queue.
*/
static size_t get_node_height(const struct avl_bs_tree_node *restrict src)
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
 * This method calculate the balance factor of a binary search tree.
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

int cjlib_dict_search(struct cjlib_json_datatype_ext *restrict dst, const struct avl_bs_tree_node *dict, 
                      const char *restrict key)
{
    struct avl_bs_tree_node *curr_node = dict;
    int compare_key = 0;
    while (curr_node) {
        compare_key = strcmp(key, dict->avl_key);

        if (compare_key > 0) {
            curr_node = curr_node->avl_right;
        } else if (compare_key < 0) {
            curr_node = curr_node->avl_left;
        } else {
            (void)memcpy(dst, curr_node->avl_data, sizeof(struct cjlib_json_datatype_ext));
            return 0;
        }
    }

    return -1;
}

int cjlib_dict_insert(const struct cjlib_json_datatype_ext *restrict src, struct avl_bs_tree_node *restrict dict,
                      const char *restrict key)
{
    // AFTER INSERTION
    if (calc_balance_factor(dict) <= 1) {
        // OK
    } else {
        // Not ok, the tree is not balanced.
    }
}

int cjlib_dict_remove(struct avl_bs_tree_node *restrict dict, const char *restrict key)
{
    // AFTER DELETION
    if (calc_balance_factor(dict) <= 1) {
        // OK
    } else {
        // Not ok, the tree is not balanced.
    }
}