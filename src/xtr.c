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
#define XTR_MIN(a, b) ((a) < (b) ? (a) : (b))
#define XTR_MAX(a, b) ((a) >= (b) ? (a) : (b))

struct xtr
{
    size_t buffer_len;
    size_t used; // BEFORE terminator
    char buffer[]; // TODO uint8_t or char
};

XTR_INLINE static size_t
allocation_size(const size_t len)
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
xtr_new_ensure(size_t len)
{
    const size_t to_allocate = allocation_size(len);
    if (did_size_overflow(to_allocate, len)) { return NULL; }
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_t* const new = calloc(to_allocate, 1U);
#else
    xtr_t* const new = malloc(to_allocate);
#endif
    if (new == NULL) { return NULL; }
    new->buffer_len = len + TERMINATOR_LEN;
    new->buffer[len] = TERMINATOR; // Keep always a termination at buffer end
    xtr_clear(new);
    return new;
}

XTR_API void
xtr_free(xtr_t** const pxtr)
{
    if (pxtr != NULL && *pxtr != NULL)
    {
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out((*pxtr)->buffer, (*pxtr)->buffer_len);
#endif
        free((*pxtr));
        *pxtr = NULL;// To avoid use-after free
    }
}

XTR_API xtr_t*
xtr_new_empty(void)
{
    return xtr_new_ensure(0);
}

XTR_API xtr_t*
xtr_new_from(const char* const str) // TODO what about uint8_t arrays?
{
    if (str == NULL) { return NULL; }
    const size_t len = strlen(str);
    xtr_t* const new = xtr_new_ensure(len);
    if (new == NULL) { return NULL; }
    strncpy(new->buffer, str, len);
    return new;
}

XTR_API xtr_t*
xtr_new_from_ensure(const char* const str, const size_t len) // TODO what about uint8_t arrays?
{
    if (str == NULL) { return NULL; }
    const size_t ensure_len = XTR_MAX(strlen(str), len);
    xtr_t* const new = xtr_new_ensure(ensure_len);
    if (new == NULL) { return NULL; }
    strncpy(new->buffer, str, ensure_len);
    return new;
}

XTR_API xtr_t*
xtr_new_clone(const xtr_t* xtr)
{
    if (xtr == NULL) { return NULL; }
    const size_t len = xtr_len(xtr);
    xtr_t* const new = xtr_new_ensure(len);
    if (new == NULL) { return NULL; }
    memcpy(new, xtr, len);
    return new;
}

XTR_API xtr_t*
xtr_new_clone_ensure(const xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL) { return NULL; }
    const size_t xlen = xtr_len(xtr);
    xtr_t* const new = xtr_new_ensure(XTR_MAX(xlen, len));
    if (new == NULL) { return NULL; }
    memcpy(new, xtr, xlen);
    return new;
}

XTR_API xtr_t*
xtr_new_fill(const char c, const size_t len)
{
    xtr_t* const new = xtr_new_ensure(len);
    if (new == NULL) { return NULL; }
    memset(new->buffer, c, len);
    new->used = len;
    return new;
}

XTR_API xtr_t*
xtr_new_repeat(const char* const str, const size_t times)
{
    if (str == NULL) { return NULL; }
    const size_t part_len = strlen(str);
    const size_t total_len = part_len * times;
    if (total_len < part_len) { return NULL; }// TODO are all overflows cases handled correctly?
    xtr_t* const new = xtr_new_ensure(total_len);
    if (new == NULL) { return NULL; }
    for (size_t i = 0; i < times; i++)
    {
        memcpy(&new->buffer[i * part_len], str, part_len);
    }
    new->used = total_len;
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
    return xtr->buffer_len;
}

XTR_API XTR_INLINE size_t
xtr_available(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->buffer_len - xtr->used;
}

XTR_API bool
xtr_is_empty(const xtr_t* const xtr)
{
    return xtr == NULL || xtr->used == 0U;
}

XTR_API xtr_t*
xtr_merge(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL) { return NULL; }
    const size_t merged_len = a->used + b->used;
    // Check for size_t overflow
    if (merged_len < a->used || merged_len < b->used) { return NULL; }
    xtr_t* const merged = xtr_new_ensure(merged_len);
    if (merged == NULL) { return NULL; }
    memcpy(merged->buffer, a->buffer, a->used);
    memcpy(merged->buffer + a->used, b->buffer, b->used);
    merged->buffer[merged_len] = TERMINATOR;
    merged->used = merged_len;
    return merged;
}

XTR_API xtr_t*
xtr_resize(xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL) { return NULL; }
    if (xtr->used > len)
    {
        // Clear bytes at the end, do keep same allocation buffer
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out(xtr->buffer + len, xtr->used - len);
#endif
        xtr->used = len;
        xtr->buffer[len] = TERMINATOR;
        return xtr;
    }
    else
    {
        // Buffer needs to be expanded, reallocate
        const size_t to_allocate = allocation_size(len);
        if (did_size_overflow(to_allocate, len)) { return NULL; }
        xtr_t* const new = realloc(xtr, to_allocate);
        if (new == NULL) { return NULL; }
        new->buffer_len = allocation_size(len);
        return new;
    }
}

XTR_API const char*
xtr_cstring(const xtr_t* const xtr)
{
    if (xtr == NULL) { return NULL; }
    else { return xtr->buffer; }
}

XTR_API int
xtr_cmp(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL) { return 0; }
    else
    {
        return strncmp(a->buffer, b->buffer, XTR_MIN(a->used, b->used));
    }
}

XTR_API size_t
xtr_find(const xtr_t* const xtr, const xtr_t* const sub)
{
    if (xtr == NULL || sub == NULL) { return 0U; } // TODO no distrinction from [0]
    if (xtr_is_empty(xtr)) { return 0U; } // TODO handle error case
    if (xtr_is_empty(sub)) { return 0U; } // TODO handle error case
    const char* const location = strnstr(
            xtr_cstring(xtr), xtr_cstring(sub), XTR_MIN(xtr->used, sub->used));
    if (location == NULL) { return 0U; } // TODO not found case
    return (size_t) (location - xtr->buffer);
}

XTR_API size_t
xtr_find_from(const xtr_t* const xtr, const xtr_t* const sub, const size_t start)
{
    // TODO
    return 0;
}

XTR_API size_t
xtr_find_in(const xtr_t* const xtr, const xtr_t* const sub, const size_t start, const size_t end)
{
    // TODO
    return 0;
}

XTR_API void
xtr_clear(xtr_t* const xtr)
{
    if (xtr != NULL)
    {
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out(xtr->buffer, xtr->buffer_len);
#endif
        xtr->used = 0U;
        xtr->buffer[0] = TERMINATOR;
    }
}

XTR_API void
xtr_rtrim(xtr_t* const xtr, const char* const chars)
{
    if (xtr_is_empty(xtr)) { return; }
    const char* last = &xtr->buffer[xtr->used - 1U]; // Not empty xtr
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (isspace(*last)) { last--; }
    }
    else
    {
        while (strchr(chars, *last)) { last--; }
    }
    const size_t new_used = (last + 1U) - xtr->buffer;
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_zero_out(&xtr->buffer[new_used], xtr->used - new_used);
#endif
    xtr->used = new_used;
    xtr->buffer[new_used] = TERMINATOR;
}

XTR_API xtr_t*
xtr_pop(xtr_t* const xtr, const size_t len){
    if (xtr == NULL) { return NULL; }
    const size_t to_pop = XTR_MIN(len, xtr->used);
    xtr_t* const popped = xtr_new_from_ensure(&xtr->buffer[xtr->used - to_pop], to_pop);
    return xtr_resize(xtr, xtr->used - to_pop);
}
