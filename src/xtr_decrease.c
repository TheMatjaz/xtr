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

XTR_API void
xtr_clear(xtr_t* const xtr)
{
    if (xtr != NULL)
    {
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
        zero_out(xtr->buffer, xtr->capacity);
#endif
        set_used_and_terminator(xtr, 0U);
    }
}

XTR_API void
xtr_truncate_head(xtr_t* const xtr, const size_t amount_to_truncate)
{
    if (xtr == NULL)
    {
        return;
    }
    const size_t to_truncate = XTR_MIN(amount_to_truncate, xtr->used);
    const size_t new_len = xtr->used - to_truncate;
    memmove_zero_out(xtr->buffer, xtr->buffer + to_truncate, new_len);
    set_used_and_terminator(xtr, new_len);
}

XTR_API void
xtr_truncate_tail(xtr_t* const xtr, const size_t amount_to_truncate)
{
    if (xtr == NULL)
    {
        return;
    }
    const size_t to_truncate = XTR_MIN(amount_to_truncate, xtr->used);
    const size_t new_len = xtr->used - to_truncate;
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
    zero_out(&xtr->buffer[new_len], amount_to_truncate);
#endif
    set_used_and_terminator(xtr, new_len);
}

XTR_API xtr_t*
xtr_pop_tail(xtr_t* const xtr, const size_t amount_to_pop)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    const size_t poppable = XTR_MIN(amount_to_pop, xtr->used);
    const size_t new_len = xtr->used - poppable;
    xtr_t* const popped = xtr_from_bytes(&xtr->buffer[new_len], poppable);
    if (popped == NULL)
    {
        return NULL;
    }
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
    zero_out(&xtr->buffer[new_len], poppable);
#endif
    set_used_and_terminator(xtr, new_len);
    return popped;
}

XTR_API xtr_t*
xtr_pop_head(xtr_t* const xtr, const size_t amount_to_pop)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    const size_t poppable = XTR_MIN(amount_to_pop, xtr->used);
    const size_t new_len = xtr->used - poppable;
    xtr_t* const popped = xtr_from_bytes(xtr->buffer, poppable);
    if (popped == NULL)
    {
        return NULL;
    }
    memmove_zero_out(xtr->buffer, xtr->buffer + poppable, new_len);
    set_used_and_terminator(xtr, new_len);
    return popped;
}

XTR_API void
xtr_trim_tail(xtr_t* xtr, const char* chars)
{
    if (xtr_is_empty(xtr))
    {
        return;
    }
    size_t last = xtr->used - 1U;  // Shifting index of last character, moving towards 0.
    // Stop when non-space found or when `last` loops around (`0--`)
    // thus the entire buffer was already explored.
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (last < xtr->used && isspace(xtr->buffer[last]))
        {
            last--;
        }
    }
    else
    {
        while (last < xtr->used && strchr(chars, xtr->buffer[last]))
        {
            last--;
        }
    }
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
    zero_out(&xtr->buffer[last], (xtr->used - 1U) - last);
#endif
    set_used_and_terminator(xtr, last);
}

XTR_API void
xtr_trim_head(xtr_t* xtr, const char* chars)
{
    if (xtr_is_empty(xtr))
    {
        return;
    }
    size_t first = 0U;  // Shifting index of first character, moving towards end.
    // Stop when non-space found or when `first` hits string end,
    // thus the entire buffer was already explored.
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (first < xtr->used && isspace(xtr->buffer[first]))
        {
            first++;
        }
    }
    else
    {
        while (first < xtr->used && strchr(chars, xtr->buffer[first]))
        {
            first++;
        }
    }
    memmove_zero_out(xtr->buffer, &xtr->buffer[first], xtr->used - first);
    set_used_and_terminator(xtr, xtr->used - first);
}

XTR_API void
xtr_trim(xtr_t* xtr, const char* chars)
{
    xtr_trim_tail(xtr, chars);
    xtr_trim_head(xtr, chars);
}

// TODO binary version? not char* but void*?
XTR_API void
xtr_truncate_suffix(xtr_t* xtr, const char* suffix)
{
    if (xtr_is_empty(xtr) || suffix == NULL)
    {
        return;
    }
    const size_t suffix_len = strlen(suffix);
    if (suffix_len > xtr->used)
    {
        return;
    }
    if (memcmp(&xtr->buffer[xtr->used - suffix_len], suffix, suffix_len) == 0)
    {
        xtr_truncate_tail(xtr, suffix_len);
    }
}

XTR_API void
xtr_truncate_prefix(xtr_t* xtr, const char* prefix)
{
    if (xtr_is_empty(xtr) || prefix == NULL)
    {
        return;
    }
    const size_t prefix_len = strlen(prefix);
    if (prefix_len > xtr->used)
    {
        return;
    }
    if (memcmp(xtr->buffer, prefix, prefix_len) == 0)
    {
        xtr_truncate_head(xtr, prefix_len);
    }
}

XTR_API xtr_t*
xtr_truncated(const xtr_t* const xtr, size_t at_most)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    if (at_most > xtr->used)
    {
        at_most = xtr->used;
    }
    xtr_t* const shorter = xtr_malloc(at_most, at_most);
    if (shorter == NULL)
    {
        return NULL;
    }
    memcpy(shorter->buffer, xtr->buffer, at_most);
    set_used_and_terminator(shorter, at_most);
    return shorter;
}

XTR_API xtr_t*
xtr_truncate(xtr_t** const pxtr, const size_t max_len)
{
    if (pxtr == NULL)
    {
        return NULL;
    }
    xtr_t* const smaller = xtr_truncated(*pxtr, max_len);
    if (smaller == NULL)
    {
        return NULL;
    }
    xtr_free(pxtr);
    *pxtr = smaller;
    return smaller;
}
