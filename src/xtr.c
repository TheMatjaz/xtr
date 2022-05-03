/**
 * @file
 * Xtring - Extendible strings for C
 *
 * @copyright Copyright © 2022, Matjaž Guštin <dev@matjaz.it>
 * <https://matjaz.it>. All rights reserved.
 * @license BSD 3-Clause License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xtr.h"

#define TERMINATOR_LEN 1U
#define TERMINATOR '\0'
#define SIZE_OVERFLOW(to_alloc, len) ((to_alloc) <= (len))

struct xtr
{
    size_t allocated;
    size_t used; // BEFORE terminator
    char buffer[]; // TODO uint8_t or char
};

XTR_INLINE static size_t
allocation_size(const size_t len)
{
    return sizeof(size_t) * 2U + TERMINATOR_LEN + len;
}

XTR_INLINE static size_t
buffer_len(const size_t len)
{
    return sizeof(size_t) * 2U + TERMINATOR_LEN + len;
}

XTR_INLINE static bool
did_size_overflow(const size_t to_allocate, const size_t len)
{
    return to_allocate <= len; // Struct size must always be > buffer len
}

XTR_INLINE static void
xtr_zero_out(void* const data, volatile const size_t len)
{
    memset(data, 0, len);
    // TODO use something the compiler can't optimise away
    // Libsodium has good ideas
}

XTR_API xtr_t*
xtr_new(const size_t len)
{
    const size_t to_allocate = allocation_size(len);
    if (did_size_overflow(to_allocate, len)) { return NULL; }
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_t* const new = calloc(to_allocate, 1U);
#else
    xtr_t* const new = malloc(to_allocate);
#endif
    if (new == NULL) { return NULL; }
    new->allocated = len + TERMINATOR_LEN;
    new->used = 0U;
    new->buffer[0] = TERMINATOR;
    return new;
}

XTR_API void
xtr_free(xtr_t** const pxtr)
{
    if (pxtr != NULL && *pxtr != NULL)
    {
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out((*pxtr)->buffer, (*pxtr)->allocated);
#endif
        free((*pxtr));
        *pxtr = NULL;// To avoid use-after free
    }
}

XTR_API xtr_t*
xtr_empty(void)
{
    return xtr_new(0);
}

XTR_API xtr_t*
xtr_from(const char* const str) // TODO what about uint8_t arrays?
{
    if (str == NULL) { return NULL; }
    const size_t len = strlen(str);
    xtr_t* const new = xtr_new(len);
    if (new == NULL) { return NULL; }
    strncpy(new->buffer, str, len);
    return new;
}

XTR_API xtr_t*
xtr_clone(const xtr_t* const xtr)
{
    if (xtr == NULL) { return NULL; }
    const size_t len = xtr_len(xtr);
    xtr_t* const new = xtr_new(len);
    if (new == NULL) { return NULL; }
    memcpy(new, xtr, len);
    return new;
}

XTR_API XTR_INLINE size_t
xtr_len(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->used;
}

XTR_API XTR_INLINE size_t
xtr_allocated(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->allocated;
}

XTR_API XTR_INLINE size_t
xtr_available(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->allocated - xtr->used;
}

XTR_API xtr_t*
xtr_merge(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL) { return NULL; }
    const size_t merged_len = a->used + b->used;
    // Check for size_t overflow
    if (merged_len < a->used || merged_len < b->used) { return NULL; }
    xtr_t* const merged = xtr_new(merged_len);
    if (merged == NULL) { return NULL; }
    memcpy(merged->buffer, a->buffer, a->used);
    memcpy(merged->buffer + a->used, b->buffer, b->used);
    merged->buffer[merged_len] = TERMINATOR;
    merged->used = merged_len;
    return merged;
}

XTR_API void
xtr_ensure_alloc(xtr_t** const pxtr, const size_t len)
{
    if (pxtr == NULL || *pxtr == NULL) { return; } // TODO No error indicator
    if ((*pxtr)->allocated >= len) { return; } // Enough space already
    const size_t to_allocate = allocation_size(len);
    if (did_size_overflow(to_allocate, len)) { return; }
    xtr_t* const new = realloc(*pxtr, to_allocate);
    if (new == NULL) { return; }
    new->allocated = len;
    xtr_free(pxtr);
    *pxtr = new;
}

// Clarify difference between trim and truncate
XTR_API void
xtr_truncate(xtr_t* const xtr, const size_t maxlen)
{
    if (xtr != NULL && xtr->used > maxlen)
    {
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out(xtr->buffer + maxlen, xtr->used - maxlen);
#endif
        xtr->used = maxlen;
    }
}

XTR_API void
xtr_set_len(xtr_t** const pxtr, const size_t len)
{
    if (pxtr == NULL || *pxtr == NULL || (*pxtr)->used == len) { return; }
    if (len < (*pxtr)->used) { xtr_truncate(*pxtr, len); }
    else
    {
        xtr_ensure_alloc(pxtr, len);
        (*pxtr)->used = len;
    }
}

XTR_API void
xtr_extend(xtr_t** const pa, const xtr_t* const b)
{
    if (pa == NULL || *pa == NULL || b == NULL) { return; } // TODO errcode?
    const size_t merged_len = (*pa)->used + b->used;
    // Check for size_t overflow
    if (merged_len < (*pa)->used || merged_len < b->used) { return; }
    xtr_ensure_alloc(pa, merged_len);
    // TODO realloc could fail
    memcpy((*pa)->buffer + b->used, b->buffer, b->used);
    (*pa)->buffer[merged_len] = TERMINATOR;
    (*pa)->used = merged_len;
}
