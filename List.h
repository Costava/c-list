/*
File: List.h
Author: Costava
License: BSD 2-Clause License (see end of file)

A List struct is used to manage the state of a dynamic array.
The array buffer is heap-allocated and self-resizing (grows when necessary).
For a given list, all elements have the same type.
Buffer size can grow linearly or exponentially (set when initializing struct).
All functions will print to stderr and exit if a memory allocation fails.

Always call _Init on a list before using it.
Call _Deinit to clean up and free the internals of a list.

If you want to use list with a certain type,
  you will need to generate the code.
There are two ways to generate list code for a given type:

1. Use macro LIST_GENERATE_FOR_TYPE in the main file of your program
     or an implementation file (when forward declarations are not needed).
   Explanation of macro parameters:
   1. type of element for the list e.g. char, struct foo, unsigned char
   2. suffix is what will follow list in the function names
       e.g. ListChar_PushBack
            when Char is given as the suffix in the macro call.

2. Use macro LIST_GENERATE_HEADER_CODE in the type's header file.
   Use macro LIST_GENERATE_IMPLEMENTATION_CODE in the type's .c file.
   Both macros have the same parameters as LIST_GENERATE_FOR_TYPE

*/

#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

// Multiply the size of the buffer by the grow_val
#define LIST_GROW_MODE_MULTIPLY 1
// Add grow_val number of spots to the buffer
#define LIST_GROW_MODE_ADD 2

////////////////////////////////////////////////////////////////////////////////

// Generates header code for the given type.
// suffix is used in the function names.
// e.g. ListInt_PopBack if suffix is Int
#define LIST_GENERATE_HEADER_CODE(type, suffix)                                \
typedef struct List##suffix {                                                  \
    /* Non-List/non-internal functions                                         \
       should NOT touch fields other than buf and length */                    \
                                                                               \
    type *buf;                                                                 \
                                                                               \
    /* Number of elements in buf */                                            \
    size_t length;                                                             \
                                                                               \
    /* Number of spots allocated for the buf */                                \
    size_t capacity;                                                           \
                                                                               \
    /* Usage depends on the grow_mode value */                                 \
    size_t grow_val;                                                           \
                                                                               \
    /* The way that the buffer size should grow when space is needed           \
     * Use the LIST_GROW_MODE_... constants to specify a value                 \
     */                                                                        \
    uint8_t grow_mode;                                                         \
} List##suffix;                                                                \
                                                                               \
/* Clean up and free any internals of given list.                              \
 */                                                                            \
void List##suffix##_Deinit(List##suffix *const list);                          \
                                                                               \
/* Do not use a List struct before calling _Init on it.                        \
 */                                                                            \
void List##suffix##_Init(List##suffix *const list, const size_t capacity,      \
    const uint8_t grow_mode, const size_t grow_val);                           \
                                                                               \
/* Insert val into given list at given index.                                  \
 * Shift values further into buf to make space if necessary.                   \
 * Print to stderr and exit(1) if error.                                       \
 * If list already has SIZE_MAX members, prints to stderr and exits.           \
 */                                                                            \
void List##suffix##_InsertAtShift(                                             \
    List##suffix *const list, const type val, const size_t index);             \
                                                                               \
/* Insert val into given list at given index.                                  \
 * Swap value at index to end of buf if necessary.                             \
 * Print to stderr and exit(1) if error.                                       \
 * If list already has SIZE_MAX members, prints to stderr and exits.           \
 */                                                                            \
void List##suffix##_InsertAtSwap(                                              \
    List##suffix *const list, const type val, const size_t index);             \
                                                                               \
/* Remove and return the value at the end of the given list.                   \
 * Print to stderr and exit if list is empty.                                  \
 */                                                                            \
type List##suffix##_PopBack(List##suffix *const list);                         \
                                                                               \
/* Append val to end of given list.                                            \
 * If list already has SIZE_MAX members, prints to stderr and exits.           \
 */                                                                            \
void List##suffix##_PushBack(List##suffix *const list, type val);              \
                                                                               \
/* Remove and return the value at the given index.                             \
 * Shift elements over to fill hole.                                           \
 * If error, print to stderr and exit(1)                                       \
 */                                                                            \
type List##suffix##_RemoveAtShift(                                             \
    List##suffix *const list, const size_t index);                             \
                                                                               \
/* Remove and return the value at the given index.                             \
 * Move the last element in buf into the space created by the removal.         \
 * If error, print to stderr and exit(1)                                       \
 */                                                                            \
type List##suffix##_RemoveAtSwap(                                              \
    List##suffix *const list, const size_t index);                             \
                                                                               \
/* Shrink buffer to the size of its current contents.                          \
 */                                                                            \
void List##suffix##_ShrinkToFit(List##suffix *const list);


// Generates implementation code for the given type.
// See above for explanation of suffix.
#define LIST_GENERATE_IMPLEMENTATION_CODE(type, suffix)                        \
/* Reallocate the buf for list based on current capacity value.                \
 * Print to stderr and exit if the bytes to allocate overflows size_t.         \
 * You should never need to call this (other list functions will).             \
 */                                                                            \
static void List##suffix##_ReallocateBuf(List##suffix *const list)             \
{                                                                              \
    const size_t new_size = sizeof(type) * list->capacity;                     \
                                                                               \
    if ((new_size / sizeof(type)) != list->capacity)                           \
    {                                                                          \
        fprintf(stderr,                                                        \
            "%s: Number of bytes to allocate overflows size_t. "               \
            "[capacity: %zu] [sizeof(type): %zu]\n", __func__,                 \
            list->capacity, sizeof(type));                                     \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    if (new_size > 0) {                                                        \
        list->buf = realloc(list->buf, new_size);                              \
                                                                               \
        if (list->buf == NULL) {                                               \
            fprintf(stderr, "%s: Failed to realloc to %zu bytes\n",            \
                __func__, new_size);                                           \
                                                                               \
            exit(1);                                                           \
        }                                                                      \
    }                                                                          \
    else {                                                                     \
        free(list->buf);                                                       \
        list->buf = NULL;                                                      \
    }                                                                          \
}                                                                              \
                                                                               \
/* Ensure list has space for at least 1 more value.                            \
 * Else print to stderr and exit if fail to make space.                        \
 * You should never need to call this (other list functions will).             \
 */                                                                            \
static void List##suffix##_EnsureSpace(List##suffix *const list)               \
{                                                                              \
    if (list->length == list->capacity)                                        \
    {                                                                          \
        if (list->capacity == SIZE_MAX) {                                      \
            fprintf(stderr, "%s: Cannot add to full, max-sized list.\n",       \
                __func__);                                                     \
                                                                               \
            exit(1);                                                           \
        }                                                                      \
                                                                               \
        switch (list->grow_mode) {                                             \
            case LIST_GROW_MODE_MULTIPLY:                                      \
            {                                                                  \
                size_t new_capacity = list->capacity * list->grow_val;         \
                if ((new_capacity / list->capacity) != list->grow_val)         \
                {                                                              \
                    /* Multiplying would overflow capacity so we have to       \
                     * settle for the maximum value.                           \
                     */                                                        \
                    new_capacity = SIZE_MAX;                                   \
                }                                                              \
                                                                               \
                list->capacity = new_capacity;                                 \
                List##suffix##_ReallocateBuf(list);                            \
            } break;                                                           \
            case LIST_GROW_MODE_ADD:                                           \
            {                                                                  \
                if (list->capacity <= SIZE_MAX - list->grow_val)               \
                {                                                              \
                    list->capacity += list->grow_val;                          \
                }                                                              \
                else                                                           \
                {                                                              \
                    list->capacity = SIZE_MAX;                                 \
                }                                                              \
                                                                               \
                List##suffix##_ReallocateBuf(list);                            \
            } break;                                                           \
            default:                                                           \
            {                                                                  \
                fprintf(stderr, "%s: Unknown grow_mode value: %d\n",           \
                    __func__, list->grow_mode);                                \
                                                                               \
                exit(1);                                                       \
            } break;                                                           \
        }                                                                      \
    }                                                                          \
    else if (list->length > list->capacity) {                                  \
        fprintf(stderr, "%s: list is in invalid state. length: %zu "           \
            "capacity: %zu\n", __func__, list->length, list->capacity);        \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
}                                                                              \
                                                                               \
void List##suffix##_Deinit(List##suffix *const list)                           \
{                                                                              \
    free(list->buf);                                                           \
    list->buf    = NULL;                                                       \
    list->length    = 0;                                                       \
    list->capacity = 0;                                                        \
}                                                                              \
                                                                               \
void List##suffix##_Init(List##suffix *const list, const size_t capacity,      \
    const uint8_t grow_mode, const size_t grow_val)                            \
{                                                                              \
    /* calloc because able to separately specify num and size. */              \
    /* i.e. calloc will have to worry about possible overflow. */              \
    if (capacity > 0) {                                                        \
        list->buf = calloc(capacity, sizeof(type));                            \
                                                                               \
        if (list->buf == NULL) {                                               \
            fprintf(stderr, "%s: Failed to calloc %zu of size %zu\n",          \
                __func__, capacity, sizeof(type));                             \
                                                                               \
            exit(1);                                                           \
        }                                                                      \
    }                                                                          \
    else {                                                                     \
        list->buf = NULL;                                                      \
    }                                                                          \
                                                                               \
    list->length    = 0;                                                       \
    list->capacity  = capacity;                                                \
    list->grow_mode = grow_mode;                                               \
                                                                               \
    if (grow_val == 0) {                                                       \
        fprintf(stderr, "%s: grow_val must be non-zero.\n", __func__);         \
        exit(1);                                                               \
    }                                                                          \
    else if (grow_mode == LIST_GROW_MODE_MULTIPLY && grow_val == 1) {          \
        fprintf(stderr, "%s: grow_val must be >=2 for "                        \
            "LIST_GROW_MODE_MULTIPLY.\n", __func__);                           \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    list->grow_val  = grow_val;                                                \
}                                                                              \
                                                                               \
void List##suffix##_InsertAtShift(                                             \
    List##suffix *const list, const type val, const size_t index)              \
{                                                                              \
    if (index > list->length) {                                                \
        fprintf(stderr, "%s: Cannot insert into list of length %zu "           \
            "at index %zu\n", __func__, list->length, index);                  \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
    else if (index == list->length) {                                          \
        List##suffix##_EnsureSpace(list);                                      \
        list->buf[index] = val;                                                \
        list->length += 1;                                                     \
    }                                                                          \
    else {                                                                     \
        List##suffix##_EnsureSpace(list);                                      \
        /* Stop at 1 above index because index may be 0 */                     \
        for (size_t i = list->length; i > index + 1; i -= 1) {                 \
            list->buf[i] = list->buf[i - 1];                                   \
        }                                                                      \
        list->buf[index + 1] = list->buf[index];                               \
        list->buf[index] = val;                                                \
        list->length += 1;                                                     \
    }                                                                          \
}                                                                              \
                                                                               \
void List##suffix##_InsertAtSwap(                                              \
    List##suffix *const list, const type val, const size_t index)              \
{                                                                              \
    if (index > list->length) {                                                \
        fprintf(stderr, "%s: Cannot insert into list of length %zu "           \
            "at index %zu\n", __func__, list->length, index);                  \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
    else if (index == list->length) {                                          \
        List##suffix##_EnsureSpace(list);                                      \
        list->buf[index] = val;                                                \
        list->length += 1;                                                     \
    }                                                                          \
    else {                                                                     \
        List##suffix##_EnsureSpace(list);                                      \
        list->buf[list->length] = list->buf[index];                            \
        list->buf[index] = val;                                                \
        list->length += 1;                                                     \
    }                                                                          \
}                                                                              \
                                                                               \
type List##suffix##_PopBack(List##suffix *const list)                          \
{                                                                              \
    if (list->length == 0) {                                                   \
        fprintf(stderr, "%s: Cannot pop from empty list.\n", __func__);        \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    list->length -= 1;                                                         \
    return list->buf[list->length];                                            \
}                                                                              \
                                                                               \
void List##suffix##_PushBack(List##suffix *const list, type val)               \
{                                                                              \
    List##suffix##_EnsureSpace(list);                                          \
    list->buf[list->length] = val;                                             \
    list->length += 1;                                                         \
}                                                                              \
                                                                               \
type List##suffix##_RemoveAtShift(                                             \
    List##suffix *const list, const size_t index)                              \
{                                                                              \
    if (index >= list->length) {                                               \
        fprintf(stderr,                                                        \
            "%s: Cannot remove at index %zu from length %zu list\n",           \
            __func__, index, list->length);                                    \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    type result = list->buf[index];                                            \
    list->length -= 1;                                                         \
    for (size_t i = index; i < list->length; i += 1) {                         \
        list->buf[i] = list->buf[i + 1];                                       \
    }                                                                          \
                                                                               \
    return result;                                                             \
}                                                                              \
                                                                               \
type List##suffix##_RemoveAtSwap(                                              \
    List##suffix *const list, const size_t index)                              \
{                                                                              \
    if (index >= list->length) {                                               \
        fprintf(stderr,                                                        \
            "%s: Cannot remove at index %zu from length %zu list\n",           \
            __func__, index, list->length);                                    \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    type result = list->buf[index];                                            \
    list->buf[index] = list->buf[list->length - 1];                            \
    list->length -= 1;                                                         \
    return result;                                                             \
}                                                                              \
                                                                               \
void List##suffix##_ShrinkToFit(List##suffix *const list)                      \
{                                                                              \
    list->capacity = list->length;                                             \
    List##suffix##_ReallocateBuf(list);                                        \
}

// Generates both header and implementation code for the given type.
// Convenience macro for use in a place like the file with the main function.
// See above for explanation of suffix.
#define LIST_GENERATE_FOR_TYPE(type, suffix)                                   \
    LIST_GENERATE_HEADER_CODE(type, suffix);                                   \
    LIST_GENERATE_IMPLEMENTATION_CODE(type, suffix);

////////////////////////////////////////////////////////////////////////////////

#endif // LIST_H

////////////////////////////////////////////////////////////////////////////////

// Note:
// Structs cannot be compared with ==
// This means that we cannot generate functions that would be expected to
//   find a certain value in the buffer, unless we are able to selectively
//   not generate these functions when the type is a struct.
//
// It would be possible to communicate some way that a certain struct
//   should be compared for equality (or a special equality check for any
//   certain type), but probably would not be worth the complexity --
//   accomplish your task by iterating across the buffer instead.

////////////////////////////////////////////////////////////////////////////////

/*
BSD 2-Clause License

Copyright 2021-2022 Costava

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
