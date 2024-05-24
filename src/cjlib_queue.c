// @file: cjlib_queue.c
#include <stdbool.h>
#include <malloc.h>

#include "cjlib_queue.h"
#include "cjlib_dictionary.h"

void cjlib_queue_deqeue(void *restrict dst, size_t size, struct cjlib_queue *restrict queue)
{
    if (NULL == queue->front) return;

    struct cjlib_queue_node *tmp = queue->front;
    queue->front = tmp->q_next;
    (void)memcpy((void *) dst, &tmp->q_data, sizeof(size));
    free(tmp);
}

bool cjlib_queue_is_empty(const struct cjlib_queue *restrict queue)
{
    return (NULL == queue->front)? true:false;
}

size_t cjlib_queue_size(const struct cjlib_queue *restrict src) 
{
    if (NULL == src->front) return 0;

    struct cjlib_queue_node *tmp = src->front;
    size_t q_size = 0;
    while (tmp) {
        tmp = tmp->q_next;
        q_size += 1;        
    }

    return q_size;
}

int cjlib_queue_enqeue(const void *restrict src, size_t size, struct cjlib_queue *restrict queue)
{
    if (NULL == src) return 0;

    struct cjlib_queue_node *new_node = (struct cjlib_queue_node *) malloc(sizeof(struct cjlib_queue_node));
    if (NULL == new_node) return -1;

    new_node->q_next = NULL;
    (void)memcpy(&new_node->q_data, (void *) src, sizeof(size));

    if (NULL == queue->front) {
        queue->front = new_node;
        queue->rear  = new_node;
    } else {
        queue->rear->q_next = new_node;
        queue->rear = new_node;
    }

    return 0;
}