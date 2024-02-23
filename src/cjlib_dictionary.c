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



static int get_node_height(const struct avl_bs_tree_node *src)
{
    
}

static int calc_balance_factor(const struct avl_bs_tree_node *restrict src)
{

}

/**
 * This function compare the two nodes, given as argument, and returns true if the
 * @src1 is greater to src2. Such a function is important for the binary tree
 * implementation.
 * 
 * @param src1 The first source binary tree node.
 * @param src2 The second source binary tree node.
 * @return True if the src1 is grater to src2, otherwise false.
*/
static inline bool compare_nodes(const struct avl_bs_tree_node *restrict src1, const struct avl_bs_tree_node *restrict src2) 
{
    return (strcmp(src1->avl_key, src2->avl_key) > 0)? true:false;
}

int cjlib_dict_search(struct cjlib_json_datatype_ext *restrict dst, const struct avl_bs_tree_node *dict, 
                      const char *restrict key)
{
    struct avl_bs_tree_node *curr_node = dict;
    int compare_key = 0;
    while (dict) {
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

}

int cjlib_dict_remove(struct avl_bs_tree_node *restrict dict, const char *restrict key)
{

}