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


XTR_API size_t
xtr_find(const xtr_t* const haystack, const xtr_t* const needle)
{
    return xtr_find_within(haystack, needle, 0U, haystack->used);
}

XTR_API size_t
xtr_find_from(const xtr_t* const haystack, const xtr_t* const needle, const size_t start)
{
    return xtr_find_within(haystack, needle, start, haystack->used);
}

XTR_API size_t
xtr_find_within(const xtr_t* haystack, const xtr_t* needle,
                size_t start, size_t end)
{
    if (xtr_is_empty(haystack) || xtr_is_empty(needle)
        || start >= haystack->used || end >= haystack->used) { return SIZE_MAX; }
    const uint8_t* const location = xtr_memmem(haystack->buffer + start,
                                               end - start, needle->buffer, needle->used);
    if (location == NULL) {return SIZE_MAX;}
    return (size_t) (location - haystack->buffer);
}

XTR_API bool
xtr_contains(const xtr_t* const haystack, const xtr_t* const needle)
{
    return xtr_find_within(haystack, needle, 0U, haystack->used) != XTR_NOT_FOUND;
}

XTR_API size_t
xtr_occurrences(const xtr_t* const haystack, const xtr_t* const needle)
{
    if (xtr_is_empty(haystack) || xtr_is_empty(needle)) { return XTR_NOT_FOUND; }
    size_t count = 0U;
    const uint8_t* prev_occurrence = haystack->buffer;
    const uint8_t* this_occurrence = NULL;
    size_t remaining_len = haystack->used;
    while (true)
    {
        this_occurrence = xtr_memmem(prev_occurrence, remaining_len,
                                     needle->buffer, needle->used);
        if (this_occurrence == NULL) { break; }
        count++;
        // TODO use ptrdiff
        remaining_len -= (this_occurrence - prev_occurrence - needle->used);
        prev_occurrence = this_occurrence + needle->used;
    }
    return count;
}
