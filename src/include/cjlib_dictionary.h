/* File: cjlib_dictionary.h Constantinos Argyriou
 *
 ************************************************************************
 * Copyright (C) 2024 Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************
 */

#ifndef CJLIB_DICTIONARY_H
#define CJLIB_DICTIONARY_H

#include "cjlib_queue.h"

#include <memory.h>
#include <stdlib.h>

struct cjlib_json_data;

/**
 * AVL Binary search tree node.
*/
struct avl_bs_tree_node
{
    struct cjlib_json_data *avl_data;   // The data that the node holds.
    char *avl_key;                      // The key of the node.
    struct avl_bs_tree_node *avl_left;  // The left child of the node.
    struct avl_bs_tree_node *avl_right; // The right child of the node.
};

typedef struct avl_bs_tree_node cjlib_dict_t;

/**
 * initialize the dictionary.
 * 
 * @param src The dictionary to initialize.
*/
static inline void cjlib_dict_init(cjlib_dict_t *restrict src)
{
    (void) memset(src, 0x0, sizeof(cjlib_dict_t));
}

/**
 * Create a new dictionary
*/
static inline cjlib_dict_t *cjlib_make_dict(void)
{
    return (cjlib_dict_t *) malloc(sizeof(cjlib_dict_t));
}

/**
 * Travel through the whole tree using the POST-ORDER method. For each node execute the
 * action callback.
 *
 * @param src A pointer to the dictionary.
 * @return -1 in case an error occur, otherwise -1.
*/
extern int cjlib_dict_postorder(struct cjlib_queue *restrict dst, const cjlib_dict_t *src);

/**
 * Searches for an element in a dictionary based on its associated key.
 * 
 * @param dst  A pointer to the memory location where the data of the found element
 *             will be copied.
 * @param dict A pointer to the dictionary.
 * @param key  A pointer to a constant character string representing the key.
 * @return 0 on success, -1 otherwise.
*/
extern int cjlib_dict_search
(struct cjlib_json_data *restrict dst, const cjlib_dict_t *restrict dict,
 const char *restrict key);

/**
 * Inserts a new element with the specified key into a dictionary.
 * 
 * @param src  A pointer to the `cjlib_json_data` structure containing the data to be inserted.
 * @param dict A pointer to the dictionary where the element will be added.
 * @param key  A pointer to a constant character string representing the key.
 * @return 0 on success, -1 otherwise.
*/
extern int cjlib_dict_insert
(const struct cjlib_json_data *restrict src, cjlib_dict_t **dict,
 const char *restrict key);

/**
 * Removes an element from a CJLib dictionary based on its key.
 * 
 * @param dict A pointer to the dictionary from which the element will be removed.
 * @param key  A pointer to a constant character string representing the key.
 * @return 0 on success, -1 otherwise.
*/
extern int cjlib_dict_remove(cjlib_dict_t **dict, const char *restrict key);

/**
 * This function free's the space of all the nodes in the
 * AVL tree.
 * @param dict A pointer to the dictionary.
 * @return The height of the dictinary (a feature that came as after affect due to the implemantation).
*/
extern size_t cjlib_dict_destroy(cjlib_dict_t *dict);

#endif
