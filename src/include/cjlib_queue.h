// @file: cjlib_queue.h, linked queue implementation.
#ifndef CJLIB_QUEUE
#define CJLIB_QUEUE

#include <memory.h>
#include <stdbool.h>

#include "cjlib_dictionary.h"

/**
 * This structures represent a queue node, which 
 * store's the informations about a avl tree node.
*/
struct cjlib_queue_node 
{
    struct avl_bs_tree_node data;
    struct cjlib_queue_node *next;
};

/**
 * This structure represent a queue.
*/
struct cjlib_queue
{
    struct cjlib_queue_node *front;
    struct cjlib_queue_node *rear;
};

/**
 * This inline function initialize respectfully the memory 
 * pointed by the pointer src, which corespond to the queue 
 * of interest.
 * 
 * @param src A pointer which points to the queue of interest.
*/
static inline void cjlib_queue_init(struct cjlib_queue *restrict src)
{
    (void)memset(src, 0x0, sizeof(struct cjlib_queue));
}

/**
 * Removes the front element from a queue and stores its value.
 * 
 * @param dst A pointer to the memory location where the data from the front
 *            element of the queue will be copied.
 * @param queue A pointer to the queue from which to remove the front element.
*/
extern void cjlib_queue_deqeue(struct avl_bs_tree_node *restrict dst, struct cjlib_queue *restrict queue);

/**
 * This function check wether the queue is empty.
 * 
 * @param queue A pointer to the queue.
*/
extern bool cjlib_queue_is_empty(const struct cjlib_queue *restrict queue);

/**
 * This funciton determines the current size of the queue
 * 
 * @param src A pointer to the queue.
 * @return The size of the queue.
*/
extern size_t cjlib_queue_size(const struct cjlib_queue *restrict src);

/**
 * Adds new data to the back (rear) of a queue.
 * 
 * @param src A pointer to the data to be added to the back of the queue.
 * @param queue A pointer to the queue where the data needs to be stored.
 * @return 0 on success, othwerwise -1.
 *
*/
extern int cjlib_queue_enqeue(const struct avl_bs_tree_node *restrict src, struct cjlib_queue *restrict queue);

#endif