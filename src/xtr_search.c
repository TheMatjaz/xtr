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


XTR_API const char*
xtr_find(const xtr_t* const xtr, const xtr_t* const pattern)
{
    return xtr_find_in(xtr, pattern, 0U, xtr->used_str_len);
}

XTR_API const char*
xtr_find_from(const xtr_t* const xtr, const xtr_t* const pattern, const size_t start)
{
    return xtr_find_in(xtr, pattern, start, xtr->used_str_len);
}

XTR_API const char*
xtr_find_in(const xtr_t* const xtr, const xtr_t* const pattern,
            const size_t start, const size_t end)
{
    // Consider returning const char*
    if (xtr_is_empty(xtr) || xtr_is_empty(pattern)
        || start >= xtr->used_str_len || end >= xtr->used_str_len) { return NULL; }
    const char* const location = memmem(xtr->str_buffer + start,
                                        end - start, pattern->str_buffer, pattern->used_str_len);
    return location;
}

XTR_API bool
xtr_contains(const xtr_t* const xtr, const xtr_t* const pattern)
{
    return xtr_find_in(xtr, pattern, 0U, xtr->used_str_len) != NULL;
}

XTR_API size_t
xtr_occurrences(const xtr_t* const xtr, const xtr_t* const pattern)
{
    if (xtr_is_empty(xtr) || xtr_is_empty(pattern)) { return 0U; }
    size_t count = 0U;
    const uint8_t* prev_occurrence = xtr->str_buffer;
    const uint8_t* this_occurrence = NULL;
    size_t remaining_len = xtr->used_str_len;
    do
    {
        this_occurrence = memmem(prev_occurrence, remaining_len,
                                 pattern->str_buffer, pattern->used_str_len);
        if (this_occurrence != NULL)
        {
            count++;
            // TODO use ptrdiff
            remaining_len -= (this_occurrence - prev_occurrence - pattern->used_str_len);
            prev_occurrence = this_occurrence + pattern->used_str_len;
        }
        else { break; }
    }
    while (true);
    return count;
}
