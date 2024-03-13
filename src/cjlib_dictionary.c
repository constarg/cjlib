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

// Determine if the imbalance of the tree is on the right or left subtree.
#define T_IMBALANCE_ON_LEFT(B_FACTOR) (B_FACTOR > T_TREE_HEIGHT_LEFT)
#define T_IMBALANCE_ON_RIGHT(B_FACTOR) (B_FACTOR < T_TREE_HEIGHT_RIGHT)

/**
 * Which type of rotation must be executed
 * on the imbalanced tree.
*/
enum rotation_type
{
    LL_ROTATION,
    RR_ROTATION
};

/**
 * Calculates the height of a tree/subtree using level-order traversal and frees memory (optional).
 *
 * This function employs a level-order traversal approach combined with a queue to
 * determine the height of the provided subtree (`src`). Additionally, it offers the
 * option to free the memory allocated for each traversed node if the `delete_nodes`
 * flag is set to true.
 *  
 * @param src A pointer to the root node of the subtree whose height needs to be calculated.
 * @param delete_nodes A boolean flag indicating whether to deallocate memory
 *                      for visited nodes during the traversal:
 *                      - True: Frees memory for each visited node.
 *                      - False: Does not free memory.
 * @returns The integer value representing the height of the subtree.
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
 * 
 * @param src A pointer that points to the root of the subtree.
 * @returns The height of this subtree.
*/
static inline size_t get_node_height(const struct avl_bs_tree_node *restrict src) 
{
    return lvl_order_traversal(src, T_DONT_DELETE_NODES);
}

/**
 * Calculates the balance factor of a node in an AVL tree.
 * 
 * @param src A pointer to the node in the AVL tree for which to calculate the balance factor.
 * @return An integer representing the balance factor of the node:
 *         - Positive value: The left subtree is taller.
 *         - Negative value: The right subtree is taller.
 *         - 0: The subtrees have the same height.
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
 * Retrieves a node with a specific key or its parent from an AVL tree.
 *
 * This function searches the AVL tree pointed to by `dict` for a node whose key
 * matches the provided `key`. Depending on the value of `get_parent`, it
 * returns either the node itself or its parent.
 * 
 * @param dict A pointer to the root node of the AVL tree to search.
 * @param key A pointer to a constant character string representing the key.
 * @param get_parent A boolean flag indicating whether to retrieve the parent or the node itself.
 * @returns A pointer to either:
 *          - The node that holds the key, if `get_parent` is false and a match is found.
 *          - The parent of the node that holds the key, if `get_parent` is true and a match is found.
 *          - The last encountered parent during the search, if `get_parent` is true
 *            and no node with the specified key exists. 
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
    return (get_parent == S_RETRIEVE_KEY_NODE)? curr_node:curr_parent;
}

/**
 * Searches for a node with a specific key in an AVL tree.
 * 
 * @param dst A pointer to the memory location where the data of the found node will be copied.
 * @param dict The root of the AVL tree.
 * @param key A pointer to a constant character string representing the key.
 * @return 0 on success, otherwise -1.
*/
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

/**
 * Assigns a value to a specified key within an AVL tree node.
 *
 * This function associates a given `value` with the provided `key` and stores them
 * together within the `src` node.
 * 
 * @param dst A pointer to the node where the key-value pair will be stored.
 * @param key A pointer to a constant character string representing the key.
 * @param value A pointer to a structure containing the data to be associated with the `key`.
 * @return 0 on success, otherwise -1.
*/
static inline int assign_key_value_to_node(struct avl_bs_tree_node *restrict dst, const char *restrict key,
                                           const struct cjlib_json_data *restrict value)
{
    dst->avl_key  = (char *) key;
    dst->avl_data = (struct cjlib_json_data *) malloc(sizeof(struct cjlib_json_data));
    if (NULL == dst->avl_data) return -1;

    (void)memcpy(src->avl_data, value, sizeof(struct cjlib_json_data));
    return 0;
}

/**
 * Finds the nearest ancestor of a given node in an AVL tree.
 * 
 * @param node A pointer to the node for which to find the ancestor.
 * @param dict A pointer to the root node of the AVL tree.
 * @return A pointer to the ancestor node of `node`, or it's self (if there is not ancestor).
*/
static inline struct avl_bs_tree_node *get_ancestor_node(const struct avl_bs_tree_node *restrict node,
                                                         const struct avl_bs_tree_node *restrict dict)
{
    return search_node(dict, node->avl_key, S_RETRIEVE_KEY_NODE_PARENT);
}

/**
 * Performs either a left-left (LL) or right-right (RR) rotation in an AVL tree.
 *
 * This function is used to maintain balance within an AVL tree. It takes a node
 * (`src`) as input, along with the current root of the tree (`dict`). Based on the
 * specified rotation type (`rotation`), it performs either a left-left (LL) rotation
 * or a right-right (RR) rotation around `src`. The function may update the root
 * pointer (`dict`) to reflect the new root after the rotation.
 *
 * @param src A pointer to the node to be rotated around.
 * @param dict A pointer to a pointer to the root node of the AVL tree. This value
 *              may be updated by the function.
 * @param rotation The type of rotation to perform:
 *                 - `ROTATE_LEFT`: Perform a left-left (LL) rotation.
 *                 - `ROTATE_RIGHT`: Perform a right-right (RR) rotation.
 */
static void balance_rotation(struct avl_bs_tree_node *src, struct avl_bs_tree_node **restrict dict,
                             enum rotation_type rotation)
{
    struct avl_bs_tree_node *node_A = src;
    struct avl_bs_tree_node *node_B = (rotation == LL_ROTATION)? node_A->avl_left:node_A->avl_right;
    struct avl_bs_tree_node *parent_A = get_ancestor_node(node_A, *dict);

    int compare_keys = strcmp(node_A->avl_key, parent_A->avl_key);

    // (1), (2). Replace A with B and update the linkage.
    if (T_NODE_IS_LEFT(compare_keys)) {
        parent_A->avl_left = node_B;
    } else if (T_NODE_IS_RIGHT(compare_keys)) {
        parent_A->avl_right = node_B;
    } else {
        // If keys are equal, then, there is no ancestor, A is the root of the whole tree.
        // Make B the root of the whole tree.
        *dict = node_B;
    }

    // Link the node A to the right place from B.
    if (rotation == LL_ROTATION) {
        node_B->avl_right = node_A;
    } else {
        node_B->avl_left = node_A;
    }
}

/**
 * Performs a left-left (LL) rotation in an AVL tree.
 *
 * This function adjusts the tree structure around the provided node (`src`)
 * to maintain the AVL tree's balance property.
 *
 * @param src  A pointer to the node to be rotated around.
 * @param dict A pointer to a pointer to the root node of the AVL tree.
 */
static inline void ll_rotation(struct avl_bs_tree_node *src, struct avl_bs_tree_node **restrict dict)
{
    /**    |            |
     *     A            B
     *    /           /   \
     *   B    -->    C     A
     *  /
     * C
     * Steps.
     * 1. Replace A with B (root of the left subtree).
     * 2. Put A on the right of B (by changing the linkage).
     * 3. Change, if exists, the link to the ancestors, which before the rotation was linked to A.
     * (This comments help me visualize the tree)
    */
    balance_rotation(src, dict, LL_ROTATION);
}

/**
 * Performs a right-right (RR) rotation in an AVL tree.
 *
 * This function adjusts the tree structure around the provided node (`src`)
 * to maintain the AVL tree's balance property.
 *
 * @param src  A pointer to the node to be rotated around.
 * @param dict A pointer to a pointer to the root node of the AVL tree.
 */
static inline void rr_rotation(struct avl_bs_tree_node *restrict src, struct avl_bs_tree_node **restrict dict)
{
    /**
     *  |               |
     *  A               B
     *   \            /   \
     *    B     -->  A     C
     *     \
     *      C
     * 
     * Steps.
     * 1. Replace A with B (root of the right subtree).
     * 2. Put A on the left of B.
     * 3. Change, if exists, the link to the ancestors, which before the rotation was linked to A.
     * (This comments help me visualize the tree)
    */
    balance_rotation(src, dict, RR_ROTATION);
}

/**
 * Performs a right-left (RL) rotation in an AVL tree.
 *
 * This function adjusts the tree structure around the provided node (`src`)
 * to maintain the AVL tree's balance property.
 *
 * @param src  A pointer to the node to be rotated around.
 * @param dict A pointer to a pointer to the root node of the AVL tree.
 */
static inline void rl_rotation(struct avl_bs_tree_node *src, struct avl_bs_tree_node **restrict dict)
{
    struct avl_bs_tree_node *node_A = src;
    struct avl_bs_tree_node *node_B = node_A->avl_left;
    rr_rotation(node_B, dict);
    ll_rotation(node_A, dict);
}

/**
 * Performs a left-right (LR) rotation in an AVL tree.
 *
 * This function adjusts the tree structure around the provided node (`src`)
 * to maintain the AVL tree's balance property.
 *
 * @param src  A pointer to the node to be rotated around.
 * @param dict A pointer to a pointer to the root node of the AVL tree.
 */
static inline void lr_rotation(struct avl_bs_tree_node *restrict src, struct avl_bs_tree_node **restrict dict)
{
    struct avl_bs_tree_node *node_A = src;
    struct avl_bs_tree_node *node_B = node_A->avl_right;
    ll_rotation(node_B, dict);
    rr_rotation(node_A, dict);
}

/**
 * Restores the balance of an AVL tree after a node insertion.
 * 
 * @param new_node_parent A pointer to the parent of the newly inserted node.
 * @param dict A pointer to the root node of the AVL tree.
 * @param compared_keys An indicator specifying the relative position of the new node to its parent.
*/
static inline void perform_rotation_after_insert(struct avl_bs_tree_node *restrict new_node_parent, 
                                                 struct avl_bs_tree_node *dict, int comopared_keys)
{
    struct avl_bs_tree_node *node_A;
    int balance_factor;
    // Get the parent of parent.
    node_A = get_ancestor_node(new_node_parent, dict);
    balance_factor = calc_balance_factor(node_A);
    if (T_TREE_IS_BALANCED(balance_factor)) return;
    
    if (T_NODE_IS_LEFT(comopared_keys) && T_IMBALANCE_ON_LEFT(balance_factor)) {
        // LL rotation.
        ll_rotation(node_A, &dict);
    } else if (T_NODE_IS_RIGHT(comopared_keys) && T_IMBALANCE_ON_RIGHT(balance_factor)) {
        // RR rotation.
        rr_rotation(node_A, &dict);
    } else if (T_NODE_IS_RIGHT(comopared_keys) && T_IMBALANCE_ON_LEFT(balance_factor)) {
        // RL (two rotations).
        rl_rotation(node_A, &dict);
    } else {
        // LR (two rotations).
        lr_rotation(node_A, &dict);
    }
}

int cjlib_dict_insert(const struct cjlib_json_data *restrict src, struct avl_bs_tree_node *dict,
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
    struct avl_bs_tree_node *new_node;
    int compare;
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
    perform_rotation_after_insert(parent, dict, compare);

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