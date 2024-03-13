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
#include "cjlib.h"
#include "cjlib_queue.h"

// Those constants are used in the search/insert/delete functions.
#define S_RETRIEVE_KEY_NODE        0x0
#define S_RETRIEVE_KEY_NODE_PARENT 0x1

// Those constatns are used in the lvl_order_traversal function, for the delete_nodes flag.
#define T_DONT_DELETE_NODES 0x0
#define T_DELETE_NODES      0x1

// Those macros are used to determine if the AVL is balanced.
#define T_TREE_HEIGHT_LEFT   0x1
#define T_TREE_HEIGHT_RIGHT -0x1
#define T_TREE_IS_BALANCED(B_FACTOR) (B_FACTOR <= T_TREE_HEIGHT_LEFT && B_FACTOR >= T_TREE_HEIGHT_RIGHT)

// Determine the direction of a node.
#define T_NODE_IS_LEFT(COMP)  COMP < 0
#define T_NODE_IS_RIGHT(COMP) COMP > 0

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

    if (0 != cjlib_queue_enqeue(src, &lvl_traversal_q)) return -1;

    while (!cjlib_queue_is_empty(&lvl_traversal_q)) {
        if (delete_nodes) {
            free(tmp->avl_data);
            free(tmp);
        }
        // NO if - statement needed to ensure that left or right child is NULL,
        // cause the queue will reject any NULL source.
        for (size_t i = 0; i < cjlib_queue_size(&lvl_traversal_q); i++) {
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
    return (int) left_subtree_h - right_subtree_h;
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
    struct avl_bs_tree_node *curr_node = (struct avl_bs_tree_node *) dict;
    struct avl_bs_tree_node *curr_parent = (struct avl_bs_tree_node *) dict;
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

static inline int assign_key_value_to_node(struct avl_bs_tree_node *restrict src, const char *restrict key,
                                           const struct cjlib_json_data *restrict value)
{
    src->avl_key  = (char *) key;
    src->avl_data = (struct cjlib_json_data *) malloc(sizeof(struct cjlib_json_data));
    if (NULL == src->avl_data) return -1;

    (void)memcpy(src->avl_data, value, sizeof(struct cjlib_json_data));
    return 0;
}

static inline struct avl_bs_tree_node *get_ancestor_node(const struct avl_bs_tree_node *restrict node,
                                                         const struct avl_bs_tree_node *restrict dict)
{
    return search_node(dict, node->avl_key, S_RETRIEVE_KEY_NODE_PARENT);
}

static inline struct avl_bs_tree_node *left_rotation(struct avl_bs_tree_node *restrict node)
{
    return NULL;
}

static inline struct avl_bs_tree_node *right_rotation(struct avl_bs_tree_node *restrict node)
{
    return NULL;
}

int cjlib_dict_insert(const struct cjlib_json_data *restrict src, struct avl_bs_tree_node *restrict dict,
                      const char *restrict key)
{
    // No root currently exists.
    if (NULL == dict->avl_key || NULL == dict->avl_data) {
        dict->avl_left = dict->avl_right = NULL;
        if (-1 == assign_key_value_to_node(dict, key, src)) return -1;
        return 0;
    }

    struct cjlib_json_data tmp;
    struct avl_bs_tree_node *parent;
    struct avl_bs_tree_node *grand_parent;
    struct avl_bs_tree_node *new_node;
    int compare;
    int balance_factor = 0;
    // A node with this key, already exists.
    if (0 == cjlib_dict_search(&tmp, dict, key)) return -1;
    // Retrieve the node that will be the parent of node that holds the key.
    parent = search_node(dict, key, S_RETRIEVE_KEY_NODE_PARENT);

    // Make the new node.
    new_node = (struct avl_bs_tree_node *) malloc(sizeof(struct avl_bs_tree_node));
    if (NULL == new_node) return -1;
    cjlib_dict_init(new_node);

    if (-1 == assign_key_value_to_node(new_node, key, src)) return -1;
    // Decide where to put the new node.
    compare = strcmp(key, parent->avl_key);
    if (T_NODE_IS_RIGHT(compare)) {
        parent->avl_right = new_node;
    } else {
        parent->avl_left = new_node;
    }

    // Get the parent of parent.
    grand_parent = get_ancestor_node(parent, dict);
    balance_factor = calc_balance_factor(grand_parent);
    if (T_TREE_IS_BALANCED(balance_factor)) return 0;

    if (balance_factor > T_TREE_HEIGHT_LEFT && T_NODE_IS_LEFT(compare)) {
    } else if (balance_factor < T_TREE_HEIGHT_RIGHT && T_NODE_IS_RIGHT(compare)) {
    } else if (balance_factor > T_TREE_HEIGHT_LEFT && T_NODE_IS_RIGHT(compare)) {
    } else {

    }

    return 0;
}

int cjlib_dict_remove(struct avl_bs_tree_node *dict, const char *restrict key)
{
    // TODO - do not forget to free the space that the cjlib_json_data allocate

    // AFTER DELETION
    if (!T_TREE_IS_BALANCED(calc_balance_factor(dict))) {
        // NOT BALANCED
    } 
    return 0;
}

size_t cjlib_dict_destroy(cjlib_dict *dict)
{
    return lvl_order_traversal(dict, T_DELETE_NODES);
}