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
#if (defined(XTR_SAFE) && XTR_SAFE)
        zero_out(xtr->str_buffer, xtr->max_str_len);
#endif
        set_used_str_len_and_terminator(xtr, 0U);
    }
}

XTR_API xtr_t*
xtr_pop(xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL) { return NULL; }
    const size_t to_pop = XTR_MIN(len, xtr->used_str_len);
    xtr_t* const popped = xtr_new_from_c_ensure(&xtr->str_buffer[xtr->used_str_len - to_pop],
                                                to_pop);
    // FIXME returning the wrong thing
    return xtr_resize(xtr, xtr->used_str_len - to_pop);
}

XTR_API void
xtr_rtrim(xtr_t* const xtr, const char* const chars)
{
    if (xtr_is_empty(xtr)) { return; }
    size_t last = xtr->used_str_len - 1U; // Shifting index of last character, moving towards 0.
    // Stop when non-space found or when `last` loops around (`0--`)
    // thus the entire str_buffer was already explored.
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (last < xtr->used_str_len && isspace(xtr->str_buffer[last])) { last--; }
    }
    else
    {
        while (last < xtr->used_str_len && strchr(chars, xtr->str_buffer[last])) { last--; }
    }
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_zero_out(&xtr->str_buffer[last], (xtr->used_str_len - 1U) - last);
#endif
    set_used_str_len_and_terminator(xtr, last);
}

XTR_API void // O(n) operation!
xtr_ltrim(xtr_t* const xtr, const char* const chars)
{
    if (xtr_is_empty(xtr)) { return; }
    size_t first = 0U; // Shifting index of first character, moving towards end.
    // Stop when non-space found or when `first` hits string end,
    // thus the entire str_buffer was already explored.
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (first < xtr->used_str_len && isspace(xtr->str_buffer[first])) { first++; }
    }
    else
    {
        while (first < xtr->used_str_len && strchr(chars, xtr->str_buffer[first])) { first++; }
    }
    memmove(xtr->str_buffer, &xtr->str_buffer[first], xtr->used_str_len - first);
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_zero_out(&xtr->str_buffer[xtr->used_str_len - first],
                 first); // first == amount of discarded
#endif
    set_used_str_len_and_terminator(xtr, xtr->used_str_len - first);
}
