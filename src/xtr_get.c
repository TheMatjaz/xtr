/**
 * @file
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

#include "xtr_internal.h"

XTR_API XTR_INLINE size_t
xtr_length(const xtr_t* xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->used;
}

XTR_API XTR_INLINE size_t
xtr_capacity(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->capacity;
}

XTR_API XTR_INLINE size_t
xtr_available(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->capacity - xtr->used;
}

XTR_API const char*
xtr_cstring(const xtr_t* const xtr)
{
    if (xtr == NULL) { return NULL; }
    else { return (const char*) xtr->buffer; }
}

XTR_API const uint8_t*
xtr_bytes(const xtr_t* xtr)
{
    if (xtr == NULL) { return NULL; }
    else { return (uint8_t*) xtr->buffer; }
}

XTR_API const uint8_t*
xtr_last(const xtr_t* const xtr)
{
    if (xtr_is_empty(xtr)) { return NULL; }
    else { return &xtr->buffer[xtr->used - 1U]; }
}

XTR_API size_t
xtr_index(const xtr_t* const xtr, const void* const substring)
{
    const uint8_t* const sub = substring;
    if (xtr == NULL || sub < xtr->buffer || sub > xtr->buffer + xtr->used) { return SIZE_MAX; }
    return (size_t) (sub - xtr->buffer);
}

XTR_API const uint8_t*
xtr_at(const xtr_t* const xtr, const size_t index)
{
    if (xtr == NULL || index >= xtr->used) { return NULL; }
    return &xtr->buffer[index];
}

typedef struct xtr_slice
{
    const uint8_t* content;
    size_t length;
} xtr_slice_t;

XTR_API const xtr_slice_t*
xtr_slice(const xtr_t* const xtr, const size_t start, size_t end)
{
    if (xtr == NULL) { return NULL; }
    xtr_slice_t* const slice = malloc(sizeof(xtr_slice_t));
    if (slice == NULL) { return NULL; }
    if (end > xtr->used) { end = xtr->used; }
    if (start >= end || start >= xtr->used)
    {
        slice->content = NULL;
        slice->length = 0U;
    }
    else
    {
        slice->content = &xtr->buffer[start];
        slice->length = end - start;
    }
    return slice;
}

XTR_API void
xtr_write(const xtr_t* const xtr, const xtr_t* const substring, const size_t start)
{
    (void) xtr;
    (void) substring;
    (void) start;
    //TODO
}
