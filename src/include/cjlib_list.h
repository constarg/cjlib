#ifndef CJLIB_LIST
#define CJLIB_LIST

#include <memory.h>

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
    (void)memset(src, 0x0, sizeof(struct cjlib_list));
}

extern int cjlib_list_append(const void *restrict src, size_t s_size, struct cjlib_list *restrict list);


extern int cjlib_list_get(void *restrict dst, size_t s_size, const struct cjlib_list *restrict list);


#endif