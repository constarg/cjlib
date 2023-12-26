// @file: cjlib_dictionary.h
#ifndef CJLIB_DICTIONARY_H
#define CJLIB_DICTIONARY_H

#include <memory.h>

#include "cjlib_dictionary.h"
#include "cjlib.h"

/**
 * AVL Binary search tree node.
*/
struct avl_bs_tree_node
{
    struct cjlib_json_datatype_ext *avl_data;   // The data that the node holds.
    char *avl_key;                              // The key of the node.
    struct avl_bs_tree_node *avl_left;          // The left child of the node.
    struct avl_bs_tree_node *avl_right;         // The right child of the node.
};

typedef struct avl_bs_tree_node cjlib_dict;

/**
 * initialize the dictionary.
 * @param src The dictionary to initialize.
*/
static inline void cjlib_dict_init(cjlib_dict *restrict src)
{
    (void)memset(src, 0x0, sizeof(cjlib_dict));
}

/**
 * This function search for an element that has an acociated key eqaul to 'key'.
 * 
 * @param dst Where to put the result of the search.
 * @param dict The dictionary.
 * @param key The key that is acociated with the element.
 * @return 0 on success, -1 otherwise.
*/
extern int cjlib_dict_search(struct cjlib_json_datatype_ext *restrict dst, const cjlib_dict *restrict dict, 
                             const char *restrict key);

/**
 * This funciton inserts a new element with key 'key' in the dictionary.
 * 
 * @param src The element to insert to the dictionary.
 * @param dict The dictionary.
 * @param key The key to acociate this element.
 * @return 0 on success, -1 otherwise.
*/
extern int cjlib_dict_insert(const struct cjlib_json_datatype_ext *restrict src, cjlib_dict *restrict dict,
                             const char *restrict key);

/**
 * This function removes an element from the dictionary, that
 * has the key 'name'
 * @param dict The dictionary.
 * @param key The key of the element.
 * @return 0 on success, -1 otherwise.
*/
extern int cjlib_dict_remove(cjlib_dict *restrict dict, const char *restrict key);

#endif