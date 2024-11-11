/* File: cjlib_list.h
*
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
 */

#ifndef CJLIB_LIST
#define CJLIB_LIST

#include <memory.h>
#include <malloc.h>

/**
 * For each implementation
 */
#define CJLIB_LIST_FOR_EACH(ITEM, LIST_PTR, TYPE)                                                                        \
    for (struct cjlib_list_node *node = (LIST_PTR)->l_head, *keep = 0x0; node != NULL; node = node->l_next, keep = 0x0)  \
        for ((ITEM) = *((TYPE *) node->l_data); keep == 0x0; keep = node)

struct cjlib_list_node
{
    void *l_data;
    struct cjlib_list_node *l_next;
};

struct cjlib_list
{
    struct cjlib_list_node *l_head;
};

static inline void cjlib_list_init(struct cjlib_list *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_list));
}

static inline struct cjlib_list *make_list(void)
{
    return (struct cjlib_list *) malloc(sizeof(struct cjlib_list));
}

extern int cjlib_list_append(const void *restrict src, size_t s_size, struct cjlib_list *list);

extern int cjlib_list_get(void *restrict dst, size_t s_size, int index, const struct cjlib_list *list);

extern int cjlib_list_destroy(struct cjlib_list *restrict src, void (*data_disposal_routine)(void *src));

#endif
