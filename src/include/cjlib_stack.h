/* File: cjlib_stack.h
 *
 * Copyright (C) 2024 Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CJLIB_STACK_H
#define CJLIB_STACK_H

#include <memory.h>

/**
 * This structure represents a node of a stack.
 */
struct cjlib_stack_node
{
    void *s_data;
    struct cjlib_stack_node *s_next;
};

/**
 * This structure represents a stack.
 */
struct cjlib_stack
{
    struct cjlib_stack_node *s_top;
};

/**
 * This function initializes the memory of a newly created stack.
 * 
 * @param src A pointer to the stack to be initialized.
 */
static inline void cjlib_stack_init(struct cjlib_stack *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_stack));
}

/**
 * Pushes the data pointed to by src onto the stack.
 * 
 * @param src    Pointer to the data to be pushed onto the stack.
 * @param s_size Size of the data type.
 * @param stack  The stack where the data will be pushed.
 * @return 0 on success, otherwise -1.
 */
extern int cjlib_stack_push(const void *restrict src, size_t s_size, struct cjlib_stack *restrict stack);

/**
 * Removes the data at the top of the stack and copies it to dst.
 * 
 * @param dst    Pointer to where the data will be stored.
 * @param d_size Size of the data type.
 * @param src    The stack from which the data will be removed.
 * @return 0 on success, otherwise -1.
 */
extern int cjlib_stack_pop(void *restrict dst, size_t d_size, struct cjlib_stack *restrict src);

/**
 * Checks whether the stack is empty or not.
 * 
 * @param src The stack of interest.
 * @return True if the stack is empty, otherwise false.
 */
extern bool cjlib_stack_is_empty(const struct cjlib_stack *restrict src);

#endif
