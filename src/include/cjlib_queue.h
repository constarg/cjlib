// @file: cjlib_queue.h, linked queue implementation.
#ifndef CJLIB_QUEUE
#define CJLIB_QUEUE

#include <memory.h>
#include <stdbool.h>

#include "cjlib_dictionary.h"

/**
 * This structure represents the data stored in a queue node.
 */
struct cjlib_queue_node
{
    void *q_data;
    struct cjlib_queue_node *q_next;
};

/**
 * This structure represents a queue.
 */
struct cjlib_queue
{
    struct cjlib_queue_node *front;
    struct cjlib_queue_node *rear;
};

/**
 * This inline function initializes the memory pointed 
 * to by @src, corresponding to the queue of interest.
 * 
 * @param src A pointer to the queue to be initialized.
 */
static inline void cjlib_queue_init(struct cjlib_queue *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_queue));
}

/**
 * Removes the data from the last position of the queue and copies it to @dst.
 * 
 * @param dst    Pointer to where the data will be stored.
 * @param d_size Size of the data type.
 * @param queue  The queue from which the data will be removed.
 */
extern void cjlib_queue_deqeue(void *restrict dst, size_t d_size, struct cjlib_queue *restrict queue);

/**
 * Checks whether the queue is empty.
 * 
 * @param queue A pointer to the queue.
 * @return True if the queue is empty, otherwise false.
 */
extern bool cjlib_queue_is_empty(const struct cjlib_queue *restrict queue);

/**
 * Determines the current size of the queue.
 * 
 * @param queue A pointer to the queue.
 * @return The current size of the queue.
 */
extern size_t cjlib_queue_size(const struct cjlib_queue *restrict src);

/**
 * Appends the data pointed to by @src to the end of the queue.
 * 
 * @param src    Pointer to the data to be stored in the queue.
 * @param s_size Size of the data type to be stored.
 * @param queue  The queue where the data will be stored.
 * @returns 0 on success, otherwise returns -1.
 */
extern int cjlib_queue_enqeue(const void *restrict src, size_t s_size, struct cjlib_queue *restrict queue);

#endif
