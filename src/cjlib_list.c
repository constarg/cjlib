/* File: cjlib_list.c
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

#include <malloc.h>
#include <memory.h>
#include <stdbool.h>

#include "cjlib_list.h"

int cjlib_list_append(const void *restrict src, size_t s_size, struct cjlib_list *list)
{
    if (NULL == list) return -1;

    struct cjlib_list_node *tmp      = NULL;
    struct cjlib_list_node *new_node = NULL;

    tmp = list->l_head;
    if (NULL == tmp) goto skip;
    while (tmp->l_next) tmp = tmp->l_next;

skip:
    new_node = (struct cjlib_list_node *) malloc(sizeof(struct cjlib_list_node));
    if (NULL == new_node) return -1;

    new_node->l_next = NULL;
    new_node->l_data = malloc(s_size);
    if (NULL == new_node->l_data) return -1;

    (void) memcpy(new_node->l_data, (void *) src, s_size);

    if (NULL == list->l_head) {
        list->l_head = new_node;
    } else {
        tmp->l_next = new_node;
    }

    return 0;
}

int cjlib_list_get(void *restrict dst, size_t s_size, int index, const struct cjlib_list *list)
{
    if (NULL == list) return -1;

    struct cjlib_list_node *tmp = list->l_head;
    int count                   = 0;
    while (tmp) {
        if (count == index) break;
        tmp = tmp->l_next;
        count++;
    }

    if (NULL == tmp) return -1;

    (void) memcpy(dst, tmp->l_data, s_size);
    return 0;
}

bool cjlib_list_is_empty(const struct cjlib_list *restrict list) 
{
    if (NULL == list->l_head) return true;

    return false;
}

int cjlib_list_destroy(struct cjlib_list *restrict src, void (*data_disposal_routine)(void *src))
{
    if (NULL == src) return -1;
    if (cjlib_list_is_empty(src)) goto cjlib_list_done;

    struct cjlib_list_node *tmp    = src->l_head;
    struct cjlib_list_node *remove = src->l_head;

    while (tmp->l_next) {
        tmp = tmp->l_next;
        data_disposal_routine(remove->l_data);
        free(remove->l_data);
        free(remove);
        remove = tmp;
    }

    data_disposal_routine(tmp->l_data);
    free(tmp->l_data);
    free(tmp);

cjlib_list_done:
    free(src);
    return 0;
}
