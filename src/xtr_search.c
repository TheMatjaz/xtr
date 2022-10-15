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


XTR_API const uint8_t*
xtr_find(const xtr_t* const haystack, const xtr_t* const needle)
{
    return xtr_find_in(haystack, needle, 0U, haystack->used);
}

XTR_API const uint8_t*
xtr_find_from(const xtr_t* const haystack, const xtr_t* const needle, const size_t start)
{
    return xtr_find_in(haystack, needle, start, haystack->used);
}

XTR_API const uint8_t*
xtr_find_in(const xtr_t* const haystack, const xtr_t* const needle,
            const size_t start, const size_t end)
{
    if (xtr_is_empty(haystack) || xtr_is_empty(needle)
        || start >= haystack->used || end >= haystack->used) { return NULL; }
    const uint8_t* const location = xtr_memmem(haystack->buffer + start,
                                               end - start, needle->buffer, needle->used);
    return location;
}

XTR_API bool
xtr_contains(const xtr_t* const haystack, const xtr_t* const pattern)
{
    return xtr_find_in(haystack, pattern, 0U, haystack->used) != NULL;
}

XTR_API size_t
xtr_occurrences(const xtr_t* const haystack, const xtr_t* const needle)
{
    if (xtr_is_empty(haystack) || xtr_is_empty(needle)) { return SIZE_MAX; }
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

typedef struct xtr_iter
{
    const xtr_t* haystack;
    const xtr_t* needle;
    ptrdiff_t progress;
} xtr_iter_t;

XTR_API xtr_iter_t*
xtr_finder(const xtr_t* const haystack, const xtr_t* const needle)
{
    if (xtr_is_empty(haystack) || xtr_is_empty(needle)) { return NULL; }
    xtr_iter_t* iterator = malloc(sizeof(xtr_iter_t));
    if (iterator == NULL) { return NULL; }
    iterator->haystack = haystack;
    iterator->needle = needle;
    iterator->progress = 0;
    return iterator;
}

XTR_API const void*
xtr_finder_next(xtr_iter_t* const iterator)
{
    if (iterator == NULL || iterator->progress < 0) { return NULL; }
    const uint8_t* const match = xtr_find_in(iterator->haystack, iterator->needle,
                                             (size_t) iterator->progress, iterator->haystack->used);
    if (match == NULL) { return NULL; }
    iterator->progress = match - iterator->haystack->buffer;
    return match;
}
