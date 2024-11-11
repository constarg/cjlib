/* File: cjlib_queue.c
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

#include "cjlib_queue.h"
#include "cjlib_dictionary.h"

void cjlib_queue_deqeue(void *restrict dst, size_t d_size, struct cjlib_queue *restrict queue)
{
    if (NULL == queue->front) return;

    struct cjlib_queue_node *tmp = queue->front;
    queue->front                 = tmp->q_next;
    (void) memcpy((void *) dst, tmp->q_data, sizeof(d_size));
    free(tmp->q_data);
    free(tmp);
}

bool cjlib_queue_is_empty(const struct cjlib_queue *restrict queue)
{
    return (NULL == queue->front) ? true : false;
}

size_t cjlib_queue_size(const struct cjlib_queue *restrict src)
{
    if (NULL == src->front) return 0;

    struct cjlib_queue_node *tmp = src->front;
    size_t q_size                = 0;
    while (tmp) {
        tmp = tmp->q_next;
        q_size += 1;
    }

    return q_size;
}

int cjlib_queue_enqeue(const void *restrict src, size_t s_size, struct cjlib_queue *restrict queue)
{
    if (NULL == src || NULL == queue) return -1;

    struct cjlib_queue_node *new_node = (struct cjlib_queue_node *) malloc(sizeof(struct cjlib_queue_node));
    if (NULL == new_node) return -1;
    new_node->q_data = malloc(s_size);
    if (NULL == new_node->q_data) return -1;

    new_node->q_next = NULL;
    (void) memcpy(new_node->q_data, (void *) src, s_size);

    if (NULL == queue->front) {
        queue->front = new_node;
        queue->rear  = new_node;
    } else {
        queue->rear->q_next = new_node;
        queue->rear         = new_node;
    }

    return 0;
}
