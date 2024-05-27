#include <stdbool.h>
#include <malloc.h>

#include "cjlib_stack.h"

void cjlib_stack_pop(void *restrict dst, size_t d_size, struct cjlib_stack *restrict src)
{
    if (NULL == src || NULL == dst) return;

    struct cjlib_stack_node *top_node = src->s_top;
    if (NULL == top_node) return;
    src->s_top = src->s_top->s_next;

    (void)memcpy(dst, top_node->s_data, d_size);
    free(top_node->s_data);
    free(top_node);
}

int cjlib_stack_push(const void *restrict src, size_t s_size, struct cjlib_stack *restrict stack)
{
    if (NULL == src || stack == NULL) return -1;

    struct cjlib_stack_node *new_node = (struct cjlib_stack_node *) malloc(sizeof(struct cjlib_stack_node));
    struct cjlib_stack_node *tmp = NULL;
    if (NULL == new_node) return -1;
    new_node->s_data = malloc(s_size);

    new_node->s_next = NULL;
    (void)memcpy(new_node->s_data, (void *) src, s_size);

    if (NULL != stack->s_top) {
        tmp = stack->s_top;
        new_node->s_next = tmp;
    }

    stack->s_top = new_node;

    return 0;
}

bool cjlib_stack_is_empty(const struct cjlib_stack *restrict src)
{
    if (NULL == src) return true;

    return (NULL == src->s_top)? true:false;
}