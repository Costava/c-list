/*
File: vec.h
Author: Costava
License: BSD 2-Clause License (see end of file)

A vec is a struct to manage the state of a dynamic array.
The array buffer is heap-allocated and self-resizing (grows when necessary).
For a given vec, all elements have the same type.
Buffer size can grow linearly or exponentially (set when initializing struct).
All functions will print to stderr and exit if a memory allocation fails.

Always call _init on a vec before using it.
Call _deinit to clean up and free the internals of a vec.

If you want to use vec with a certain type,
  you will need to generate the code.
There are two ways to generate vec code for a given type:
1. Use macro VEC_GENERATE_HEADER_CODE in the type's header file.
   Use macro VEC_GENERATE_IMPLEMENTATION_CODE in the type's .c file.
   Explanation of macro parameters:
   - type e.g. char, struct foo, unsigned char
   - suffix is what will follow vec in the function names
       e.g. vecchar_push_back
            when char is given as the suffix in the macro call.
       e.g. vecstructfoo_push_back
       e.g. vecfoo_push_back
       e.g. vecunsignedchar_push_back
       e.g. vecuchar_push_back

2. Use macro VEC_GENERATE_FOR_TYPE in the main file
     or an implementation file (when forward declarations are not needed).
   For explanation of macro parameters, see 1.
*/

#ifndef VEC_H
#define VEC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__)
// For `size_t`
#include <stddef.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

// Multiply the size of the buffer by the grow_val
#define VEC_GROW_MODE_MULTIPLY 1
// Add grow_val number of spots to the buffer
#define VEC_GROW_MODE_ADD 2

// Max permitted value of capacity member of struct.
// Leave 1 space so that a normal for loop can iterate the buf.
#define VEC_MAX_CAPACITY (UINT32_MAX - 1)

////////////////////////////////////////////////////////////////////////////////
// Macro that generates the struct and its typedef
////////////////////////////////////////////////////////////////////////////////

#define VEC_DEFINE_STRUCT(type, suffix)                                        \
typedef struct vec##suffix {                                                   \
    /* These members are expected to be touched in order to                    \
         iterate over all items in vec */                                      \
    type    *buf;                                                              \
    uint32_t length; /* Number of elements in buf */                           \
                                                                               \
    /* Do not touch the below members outside of vec functions */              \
                                                                               \
    /* The number of spots allocated for the buf */                            \
    uint32_t capacity;                                                         \
    /* The way that the buffer size should grow when space is needed */        \
    /* Use the VEC_GROW_MODE_... constants to specify a value */               \
    uint8_t  grow_mode;                                                        \
    /* Usage depends on the grow_mode value */                                 \
    uint32_t grow_val;                                                         \
} vec##suffix;

////////////////////////////////////////////////////////////////////////////////
// Macros that generate declarations and definitions of the functions
////////////////////////////////////////////////////////////////////////////////

// RNP: Return type, Name, Parameters
//      (belonging to a function)

// Clean up and free any internals of vec.
#define VEC_RNP_DEINIT(type, suffix)                                           \
    void vec##suffix##_deinit(vec##suffix *const vec)

#define VEC_DEFINE_DEINIT(type, suffix)                                        \
VEC_RNP_DEINIT(type, suffix)                                                   \
{                                                                              \
    free(vec->buf);                                                            \
    vec->buf    = NULL;                                                        \
    vec->length    = 0;                                                        \
    vec->capacity = 0;                                                         \
}

// Do not use a vec struct before calling _init on it.
#define VEC_RNP_INIT(type, suffix)                                             \
    void vec##suffix##_init(vec##suffix *const vec, const uint32_t capacity,   \
        const uint8_t grow_mode, const uint32_t grow_val)

#define VEC_DEFINE_INIT(type, suffix)                                          \
VEC_RNP_INIT(type, suffix)                                                     \
{                                                                              \
    /* calloc because able to separately specify num and size. */              \
    /* i.e. calloc will have to worry about possible overflow. */              \
    if (capacity > 0) {                                                        \
        vec->buf = calloc(capacity, sizeof(type));                             \
                                                                               \
        if (vec->buf == NULL) {                                                \
            fprintf(stderr, "%s: Failed to calloc %d of size %zu\n",           \
                __func__, capacity, sizeof(type));                             \
                                                                               \
            exit(1);                                                           \
        }                                                                      \
    }                                                                          \
    else {                                                                     \
        vec->buf = NULL;                                                       \
    }                                                                          \
                                                                               \
    vec->length    = 0;                                                        \
    vec->capacity  = capacity;                                                 \
    vec->grow_mode = grow_mode;                                                \
                                                                               \
    if (grow_val == 0) {                                                       \
        fprintf(stderr, "%s: grow_val must be non-zero.\n", __func__);         \
        exit(1);                                                               \
    }                                                                          \
    else if (grow_mode == VEC_GROW_MODE_MULTIPLY && grow_val == 1) {           \
        fprintf(stderr, "%s: grow_val must be >=2 for "                        \
            "VEC_GROW_MODE_MULTIPLY.\n", __func__);                            \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    vec->grow_val  = grow_val;                                                 \
}

// Insert val into vec at given index.
// Shift values further into buf to make space if necessary.
// Print to stderr and exit(1) if error.
// If vec already has VEC_MAX_CAPACITY members, prints to stderr and exits.
#define VEC_RNP_INSERT_AT_SHIFT(type, suffix)                                  \
    void vec##suffix##_insert_at_shift(                                        \
        vec##suffix *const vec, const type val, const uint32_t index)

#define VEC_DEFINE_INSERT_AT_SHIFT(type, suffix)                               \
VEC_RNP_INSERT_AT_SHIFT(type,suffix)                                           \
{                                                                              \
    if (index > vec->length) {                                                 \
        fprintf(stderr, "%s: Cannot insert into vec of length %d "             \
            "at index %d\n", __func__, vec->length, index);                    \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
    else if (index == vec->length) {                                           \
        vec##suffix##_ensure_space(vec);                                       \
        vec->buf[index] = val;                                                 \
        vec->length += 1;                                                      \
    }                                                                          \
    else {                                                                     \
        vec##suffix##_ensure_space(vec);                                       \
        /* Stop at 1 above index because index may be 0 */                     \
        for (uint32_t i = vec->length; i > index + 1; i -= 1) {                \
            vec->buf[i] = vec->buf[i - 1];                                     \
        }                                                                      \
        vec->buf[index + 1] = vec->buf[index];                                 \
        vec->buf[index] = val;                                                 \
        vec->length += 1;                                                      \
    }                                                                          \
}

// Insert val into vec at given index.
// Swap value at index to end of buf if necessary.
// Print to stderr and exit(1) if error.
// If vec already has VEC_MAX_CAPACITY members, prints to stderr and exits.
#define VEC_RNP_INSERT_AT_SWAP(type, suffix)                                   \
    void vec##suffix##_insert_at_swap(                                         \
        vec##suffix *const vec, const type val, const uint32_t index)

#define VEC_DEFINE_INSERT_AT_SWAP(type, suffix)                                \
VEC_RNP_INSERT_AT_SWAP(type,suffix)                                            \
{                                                                              \
    if (index > vec->length) {                                                 \
        fprintf(stderr, "%s: Cannot insert into vec of length %d "             \
            "at index %d\n", __func__, vec->length, index);                    \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
    else if (index == vec->length) {                                           \
        vec##suffix##_ensure_space(vec);                                       \
        vec->buf[index] = val;                                                 \
        vec->length += 1;                                                      \
    }                                                                          \
    else {                                                                     \
        vec##suffix##_ensure_space(vec);                                       \
        vec->buf[vec->length] = vec->buf[index];                               \
        vec->buf[index] = val;                                                 \
        vec->length += 1;                                                      \
    }                                                                          \
}

// Remove and return the value at the end of the vec.
// Print to stderr and exit if vec is empty.
#define VEC_RNP_POP_BACK(type, suffix)                                         \
    type vec##suffix##_pop_back(vec##suffix *const vec)

#define VEC_DEFINE_POP_BACK(type, suffix)                                      \
VEC_RNP_POP_BACK(type, suffix)                                                 \
{                                                                              \
    if (vec->length == 0) {                                                    \
        fprintf(stderr, "%s: Cannot pop from empty vec.\n", __func__);         \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    vec->length -= 1;                                                          \
    return vec->buf[vec->length];                                              \
}

// Append val to end of vec.
// If vec already has VEC_MAX_CAPACITY members, prints to stderr and exits.
#define VEC_RNP_PUSH_BACK(type, suffix)                                        \
    void vec##suffix##_push_back(vec##suffix *const vec, type val)

#define VEC_DEFINE_PUSH_BACK(type, suffix)                                     \
VEC_RNP_PUSH_BACK(type, suffix)                                                \
{                                                                              \
    vec##suffix##_ensure_space(vec);                                           \
    vec->buf[vec->length] = val;                                               \
    vec->length += 1;                                                          \
}

// Remove and return the value at the given index.
// Shift elements over to fill hole.
// If error, print to stderr and exit(1)
#define VEC_RNP_REMOVE_AT_SHIFT(type, suffix)                                  \
    type vec##suffix##_remove_at_shift(                                        \
        vec##suffix *const vec, const uint32_t index)

#define VEC_DEFINE_REMOVE_AT_SHIFT(type, suffix)                               \
VEC_RNP_REMOVE_AT_SHIFT(type, suffix)                                          \
{                                                                              \
    if (index >= vec->length) {                                                \
        fprintf(stderr, "%s: Cannot remove at index %d from length %d vec\n",  \
            __func__, index, vec->length);                                     \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    type result = vec->buf[index];                                             \
    vec->length -= 1;                                                          \
    for (uint32_t i = index; i < vec->length; i += 1) {                        \
        vec->buf[i] = vec->buf[i + 1];                                         \
    }                                                                          \
    return result;                                                             \
}

// Remove and return the value at the given index.
// Move the last element in buf into the space created by the removal.
// If error, print to stderr and exit(1)
#define VEC_RNP_REMOVE_AT_SWAP(type, suffix)                                   \
    type vec##suffix##_remove_at_swap(                                         \
        vec##suffix *const vec, const uint32_t index)

#define VEC_DEFINE_REMOVE_AT_SWAP(type, suffix)                                \
VEC_RNP_REMOVE_AT_SWAP(type, suffix)                                           \
{                                                                              \
    if (index >= vec->length) {                                                \
        fprintf(stderr, "%s: Cannot remove at index %d from length %d vec\n",  \
            __func__, index, vec->length);                                     \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    type result = vec->buf[index];                                             \
    vec->buf[index] = vec->buf[vec->length - 1];                               \
    vec->length -= 1;                                                          \
    return result;                                                             \
}

// Shrink buffer to the size of its current contents.
#define VEC_RNP_SHRINK_TO_FIT(type, suffix)                                    \
    void vec##suffix##_shrink_to_fit(vec##suffix *const vec)

#define VEC_DEFINE_SHRINK_TO_FIT(type, suffix)                                 \
VEC_RNP_SHRINK_TO_FIT(type, suffix)                                            \
{                                                                              \
    vec->capacity = vec->length;                                               \
    vec##suffix##_reallocate_buf(vec);                                         \
}

////////////////////////////////////////////////////////////////////////////////
// Functions to be called only by other vec functions
////////////////////////////////////////////////////////////////////////////////

// Ensure vec has space for at least 1 more value.
// Else print to stderr and exit if fail to make space.
// You should never need to call this (other vec functions will).
#define VEC_RNP_ENSURE_SPACE(type, suffix)                                     \
    void vec##suffix##_ensure_space(vec##suffix *const vec)

#define VEC_DEFINE_ENSURE_SPACE(type, suffix)                                  \
VEC_RNP_ENSURE_SPACE(type, suffix)                                             \
{                                                                              \
    if (vec->length >= vec->capacity)                                          \
    {                                                                          \
        if (vec->capacity == VEC_MAX_CAPACITY) {                               \
            fprintf(stderr, "%s: Cannot add to full, max-sized vec.\n",        \
                __func__);                                                     \
                                                                               \
            exit(1);                                                           \
        }                                                                      \
                                                                               \
        switch (vec->grow_mode) {                                              \
            case VEC_GROW_MODE_MULTIPLY:                                       \
            {                                                                  \
                uint32_t new_capacity = vec->capacity * vec->grow_val;         \
                if ((new_capacity / vec->capacity) != vec->grow_val)           \
                {                                                              \
                    /* Multiplying would overflow capacity so we have to */    \
                    /*  settle for the maximum value. */                       \
                    new_capacity = VEC_MAX_CAPACITY;                           \
                }                                                              \
                                                                               \
                vec->capacity = new_capacity;                                  \
                vec##suffix##_reallocate_buf(vec);                             \
            } break;                                                           \
            case VEC_GROW_MODE_ADD:                                            \
            {                                                                  \
                if (vec->capacity <= VEC_MAX_CAPACITY - vec->grow_val)         \
                {                                                              \
                    vec->capacity += vec->grow_val;                            \
                }                                                              \
                else                                                           \
                {                                                              \
                    vec->capacity = VEC_MAX_CAPACITY;                          \
                }                                                              \
                                                                               \
                vec##suffix##_reallocate_buf(vec);                             \
            } break;                                                           \
            default:                                                           \
            {                                                                  \
                fprintf(stderr, "%s: Unknown grow_mode value: %d\n",           \
                    __func__, vec->grow_mode);                                 \
                                                                               \
                exit(1);                                                       \
            } break;                                                           \
        }                                                                      \
    }                                                                          \
}

// Reallocate the buf for vec based on current capacity value.
// Print to stderr and exit if the bytes to allocate overflows size_t.
// You should never need to call this (other vec functions will).
#define VEC_RNP_REALLOCATE_BUF(type, suffix)                                   \
    void vec##suffix##_reallocate_buf(vec##suffix *const vec)

#define VEC_DEFINE_REALLOCATE_BUF(type, suffix)                                \
VEC_RNP_REALLOCATE_BUF(type, suffix)                                           \
{                                                                              \
    const size_t new_size = sizeof(type) * vec->capacity;                      \
                                                                               \
    if ((new_size / sizeof(type)) != vec->capacity)                            \
    {                                                                          \
        fprintf(stderr, "%s: Number of bytes to allocate overflows size_t. "   \
            "[capacity: %d] [sizeof(type): %lu]\n", __func__, vec->capacity,   \
            sizeof(type));                                                     \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    if (new_size > 0) {                                                        \
        vec->buf = realloc(vec->buf, new_size);                                \
                                                                               \
        if (vec->buf == NULL) {                                                \
            fprintf(stderr, "%s: Failed to realloc to %zu bytes\n",            \
                __func__, new_size);                                           \
                                                                               \
            exit(1);                                                           \
        }                                                                      \
    }                                                                          \
    else {                                                                     \
        free(vec->buf);                                                        \
        vec->buf = NULL;                                                       \
    }                                                                          \
}

////////////////////////////////////////////////////////////////////////////////

// Other possible functions:
// reserve_at_shift
// reserve_at_swap

// Note:
// Structs cannot be compared with ==
// This means that we cannot generate functions that would be expected to
//   find a certain value in the buffer, unless we are able to selectively
//   not generate these functions when the type is a struct.
//
// It would be possible to communicate some way that a certain struct
//   should be compared for equality (or a special equality check for any
//   certain type), but probably would not be worth the complexity -- just
//   accomplish your task by iterating across the buffer instead.

////////////////////////////////////////////////////////////////////////////////
// Helper macros
////////////////////////////////////////////////////////////////////////////////

#define VEC_DECLARE_M(type, suffix, func_id)                                   \
    extern VEC_RNP_##func_id(type, suffix);

#define VEC_DEFINE_M(type, suffix, func_id)                                    \
    VEC_DEFINE_##func_id(type, suffix)

#define VEC_FOR_ALL_FUNCS(type, suffix, macro)                                 \
    macro##_M(type, suffix, DEINIT)                                            \
    macro##_M(type, suffix, INIT)                                              \
    macro##_M(type, suffix, INSERT_AT_SHIFT)                                   \
    macro##_M(type, suffix, INSERT_AT_SWAP)                                    \
    macro##_M(type, suffix, POP_BACK)                                          \
    macro##_M(type, suffix, PUSH_BACK)                                         \
    macro##_M(type, suffix, REMOVE_AT_SHIFT)                                   \
    macro##_M(type, suffix, REMOVE_AT_SWAP)                                    \
    macro##_M(type, suffix, SHRINK_TO_FIT)                                     \
                                                                               \
    /* Functions to be called only by other vec functions. */                  \
    macro##_M(type, suffix, ENSURE_SPACE)                                      \
    macro##_M(type, suffix, REALLOCATE_BUF)                                    \

////////////////////////////////////////////////////////////////////////////////
// Library-user macros
// Macros to be used by a user of this library
////////////////////////////////////////////////////////////////////////////////

// Generates header code for the given type.
// suffix is used in the function names.
// e.g. vecint_pop_back if suffix is init
// e.g. vecunsignedchar_push_back if suffix is unsignedchar
#define VEC_GENERATE_HEADER_CODE(type, suffix)                                 \
    VEC_DEFINE_STRUCT(type, suffix)                                            \
    VEC_FOR_ALL_FUNCS(type, suffix, VEC_DECLARE)

// Generates implementation code for the given type.
// See above for explanation of suffix.
#define VEC_GENERATE_IMPLEMENTATION_CODE(type, suffix)                         \
    VEC_FOR_ALL_FUNCS(type, suffix, VEC_DEFINE)

// Generates both header and implementation code for the given type.
// Convenience macro for use in a place like the file with the main function.
// See above for explanation of suffix.
#define VEC_GENERATE_FOR_TYPE(type, suffix)                                    \
    VEC_GENERATE_HEADER_CODE(type, suffix)                                     \
    VEC_GENERATE_IMPLEMENTATION_CODE(type, suffix)

////////////////////////////////////////////////////////////////////////////////

#endif // VEC_H

////////////////////////////////////////////////////////////////////////////////

/*
BSD 2-Clause License

Copyright 2021 Costava

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
