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

#define MAX_NEEDLES 10U

XTR_API xtr_t**
xtr_split(size_t* const amount_of_chunks, const xtr_t* const haystack, const xtr_t* const needle)
{
    if (amount_of_chunks == NULL) { return NULL; }
    const size_t* matches = xtr_find_all(haystack, needle);
    if (matches == NULL) { return NULL; }
    size_t chunk_idx = 0U;
    xtr_t** chunks = calloc((matches[0] + 1U), sizeof(size_t));
    if (chunks == NULL) { goto rollback; }
    if (matches[0] == 0U)
    {
        chunks[chunk_idx] = xtr_clone(haystack);
        if (chunks[chunk_idx] == NULL) { goto rollback; }
        chunk_idx++;
    }
    else
    {
        // First chunk, from start to first match
        chunks[chunk_idx] = xtr_from_bytes(&haystack->buffer[0], matches[1]);
        if (chunks[chunk_idx] == NULL) { goto rollback; }
        chunk_idx++;
        // Middle chunks, between matches
        for (size_t i = 1U; i <= matches[0] - 1U; i++)
        {
            const size_t chunk_start = matches[i] + needle->used;
            const size_t chunk_len = matches[i + 1U] - chunk_start;
            chunks[chunk_idx] = xtr_from_bytes(&haystack->buffer[start], chunk_len);
            if (chunks[chunk_idx] == NULL) { goto rollback; }
            chunk_idx++;
        }
        // Last chunk, from last match to end
        const size_t chunk_start = matches[matches[0]] + needle->used;
        const size_t chunk_len = haystack->used - chunk_start;
        chunks[chunk_idx] = xtr_from_bytes(&haystack->buffer[start], chunk_len);
        if (chunks[chunk_idx] == NULL) { goto rollback; }
        chunk_idx++;
    }
    free(matches);
    matches = NULL;
    *amount_of_chunks = chunk_idx;
    return chunks;
    rollback;
    {
        free(matches);
        matches = NULL;
        if (chunks != NULL)
        {
            while (chunk_idx) { xtr_free(&chunks[chunk_idx--]); }
            free(chunks);
            chunks = NULL;
        }
        return NULL;
    }
}

XTR_API xtr_t**
xtr_split_every(size_t* const parts, const xtr_t* const xtr, const size_t chunk_len)
{
    if (xtr_is_empty(xtr) || chunk_len == 0U) { return NULL; }
    const size_t amount_of_chunks = (xtr->used + chunk_len - 1U) / chunk_len;
    if (amount_of_chunks * chunk_len < xtr->used) { return NULL; } // Size overflow
    xtr_t** const chunks = malloc(amount_of_chunks * sizeof(xtr_t*));
    if (chunks == NULL) { return NULL; }
    xtr_t* chunk;
    size_t chunk_idx;
    for (chunk_idx = 0U; chunk_idx < amount_of_chunks - 1U; chunk_idx++)
    {
        chunk = xtr_from_bytes(&xtr->buffer[chunk_idx * chunk_len], chunk_len);
        if (chunk == NULL) { goto rollback; }
    }
    // Last chunk, may be shorter
    const size_t remaining_len = XTR_MIN(chunk_len, xtr->used - chunk_idx * chunk_len);
    chunk = xtr_from_bytes(&xtr->buffer[xtr->used - chunk_len], remaining_len);
    if (chunk == NULL) { goto rollback; }
    if (parts != NULL) { *parts = amount_of_chunks; }
    return chunks;
    rollback:
    {
        while (chunk_idx) { xtr_free(&chunks[chunk_idx--]); }
        free(chunks);
        return NULL;
    }
}
