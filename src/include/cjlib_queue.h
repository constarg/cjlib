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
 * This fucntion removes an node from the queue and returns the value that 
 * was in this node in the @param dst. After the call of this function is 
 * garanteed that the variable which has the pointer dst will have the 
 * value that corespond to the front node of the queue.
 * 
 * @param dst A pointer that points the place where the data must be stored.
 * @param qeueu A pointer to the queue of interest.
*/
extern void cjlib_queue_deqeue(struct avl_bs_tree_node *restrict dst, struct cjlib_queue *restrict queue);

/**
 * This function check wether the queue is empty.
 * 
 * @param queue A pointer to the queue of interest..
*/
extern bool cjlib_queue_is_empty(const struct cjlib_queue *restrict queue);

/**
 * This funciton determines the current size of the queue
 * @param src A pointer that points to the queue of interest
 * @return The size of that the queue given in @src.
*/
extern size_t cjlib_queue_size(const struct cjlib_queue *restrict src);

/**
 * This function store new data in the rear of the queue. After the call of this
 * function the data that are pointed by the @param src, will be placed in the 
 * back of the queue, rear. 
 * 
 * @param src A pointer that points to the data to be stored.
 * @param queue A pointer to the queue of interest.
 * @return On error -1, otherwise 0.
*/
extern int cjlib_queue_enqeue(const struct avl_bs_tree_node *restrict src, struct cjlib_queue *restrict queue);

#endif