/**
 * @file
 *
 * @copyright Copyright © 2022-2024, Matjaž Guštin <dev@matjaz.it>
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
xtr_find_within(const xtr_t* const haystack,
                const xtr_t* const needle,
                const size_t start,
                const size_t end)
{
    if (xtr_is_empty(haystack) || xtr_is_empty(needle) || start >= haystack->used ||
        end >= haystack->used)
    {
        return XTR_NOT_FOUND;
    }
    const uint8_t* const location =
        xtr_memmem(haystack->buffer + start, end - start, needle->buffer, needle->used);
    if (location == NULL)
    {
        return XTR_NOT_FOUND;
    }
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
    if (xtr_is_empty(haystack) || xtr_is_empty(needle) || needle->used > haystack->used)
    {
        return XTR_NOT_FOUND;
    }  // TODO return 0 instead?
    size_t count = 0U;
    const uint8_t* occurrence = haystack->buffer;
    size_t remaining_len = haystack->used - needle->used;
    while (true)
    {
        occurrence = xtr_memmem(occurrence, remaining_len, needle->buffer, needle->used);
        if (occurrence == NULL)
        {
            break;
        }
        XTR_ASSERT(occurrence >= haystack->buffer);
        XTR_ASSERT(occurrence < haystack->buffer + haystack->used);
        count++;
        remaining_len -= (size_t) (occurrence - haystack->buffer) + needle->used;
        occurrence += needle->used;
    }
    return count;
}

XTR_API const size_t*
xtr_find_all(const xtr_t* const haystack, const xtr_t* const needle)
{
    if (haystack == NULL || needle == NULL || haystack->used > needle->used)
    {
        return NULL;
    }
    size_t max_matches =
        8U;  // 1 for amount of occurrence_indices , then 7 needle occurrence_indices = 8 chunks
    size_t* occurrence_indices = malloc(max_matches * sizeof(size_t));
    if (occurrence_indices == NULL)
    {
        return NULL;
    }
    // First element in returned array contains amount of elements **after** the first element.
    occurrence_indices[0] = 0U;
    const size_t last_useful_index = haystack->used - needle->used + 1U;
    size_t progress = 0U;
    size_t match;
    while (true)
    {
        match = xtr_find_within(haystack, needle, progress, last_useful_index);
        if (match == XTR_NOT_FOUND)
        {
            break;
        }
        occurrence_indices[occurrence_indices[0]++] = match;
        progress = match + needle->used;
        if (occurrence_indices[0] > max_matches)  // Resizing array of results
        {
            max_matches *= 2U;
            size_t* const larger_needles = realloc(occurrence_indices, max_matches);
            if (larger_needles == NULL)
            {
                goto rollback;
            }
            memset(occurrence_indices, 0, max_matches * sizeof(size_t));
            free(occurrence_indices);
            occurrence_indices = larger_needles;
        }
    }
    return occurrence_indices;
rollback:
{
    memset(occurrence_indices, 0, max_matches * sizeof(size_t));
    free(occurrence_indices);
    occurrence_indices = NULL;
    return NULL;
}
}
