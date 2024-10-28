#include <malloc.h>
#include <memory.h>

#include "cjlib_list.h"

extern int cjlib_list_append(const void *restrict src, size_t s_size, struct cjlib_list *list)
{
    if (NULL == list) return -1;

    struct cjlib_list_node *tmp = NULL;
    struct cjlib_list_node *new_node = 0;

    tmp = list->l_head;
    if (NULL == tmp) goto skip;
    while (tmp->l_next) tmp = tmp->l_next;

skip:
    new_node = (struct cjlib_list_node *) malloc(sizeof(struct cjlib_list_node));
    if (NULL == new_node) return -1;

    new_node->l_next = NULL;
    new_node->l_data = malloc(s_size);
    if (NULL == new_node->l_data) return -1;

    (void)memcpy(new_node->l_data, src, s_size);

    if (NULL == list->l_head) {
        list->l_head = new_node;
    } else {
        tmp->l_next = new_node;
    }

    return 0;
}

extern int cjlib_list_get(void *restrict dst, size_t s_size, int index , const struct cjlib_list *list)
{
    if (NULL == list) return -1;

    struct cjlib_list_node *tmp = list->l_head;
    int count = 0;
    while (tmp) {
        if (count == index) break;
        tmp = tmp->l_next;
        count++;
    }

    if (NULL == tmp) return -1;

    (void)memcpy(dst, tmp->l_data, s_size);
    return 0;
}

extern int cjlib_list_destroy(struct cjlib_list *restrict src)
{
    if (NULL == src) return -1;
    struct cjlib_list_node *tmp = src->l_head;
    struct cjlib_list_node *remove = src->l_head;
    
    while (tmp->l_next) {
        tmp = tmp->l_next;
        free(remove->l_data);
        free(remove);
        remove = tmp;
    }

    free(tmp->l_data);
    free(tmp);

    src->l_head = NULL;
    return 0;
}
