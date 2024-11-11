/* File: cjlib_stack.c
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

#include <stdbool.h>
#include <malloc.h>

#include "cjlib_stack.h"

int cjlib_stack_pop(void *restrict dst, size_t d_size, struct cjlib_stack *restrict src)
{
    if (NULL == src || NULL == dst) return -1;

    struct cjlib_stack_node *top_node = src->s_top;
    if (NULL == top_node) return -1;
    src->s_top = src->s_top->s_next;

    (void) memcpy(dst, top_node->s_data, d_size);
    free(top_node->s_data);
    free(top_node);

    return 0;
}

int cjlib_stack_push(const void *restrict src, size_t s_size, struct cjlib_stack *restrict stack)
{
    if (NULL == src || stack == NULL) return -1;

    struct cjlib_stack_node *new_node = (struct cjlib_stack_node *) malloc(sizeof(struct cjlib_stack_node));
    struct cjlib_stack_node *tmp      = NULL;
    if (NULL == new_node) return -1;
    new_node->s_data = malloc(s_size);

    new_node->s_next = NULL;
    (void) memcpy(new_node->s_data, (void *) src, s_size);

    if (NULL != stack->s_top) {
        tmp              = stack->s_top;
        new_node->s_next = tmp;
    }

    stack->s_top = new_node;

    return 0;
}

bool cjlib_stack_is_empty(const struct cjlib_stack *restrict src)
{
    if (NULL == src) return true;

    return (NULL == src->s_top) ? true : false;
}
