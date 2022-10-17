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

static void
xtr_extend_raw(xtr_t** const pbase, const uint8_t* const part,
               const size_t part_len, const size_t repetitions)
{
    const size_t total_len = ((*pbase)->used + part_len) * repetitions;
    // Check for size_t overflow
    if (total_len < (*pbase)->used || total_len < part_len) { return; }
    // TODO not all overflow  cases are checkde
    xtr_t* const resized = xtr_resize_free(pbase, total_len);
    if (resized == NULL) { return; }
    for (size_t i = 0U; i < repetitions; i++)
    {
        memcpy(&resized->buffer[i * part_len], part, part_len);
    }
    set_used_and_terminator(resized, total_len);
}

XTR_API void
xtr_extend(xtr_t** const pbase, const xtr_t* ext)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    xtr_extend_raw(pbase, ext->buffer, ext->used, 1);
}

XTR_API void
xtr_extend_many(xtr_t** const pbase, const xtr_t* ext, const size_t repetitions)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    xtr_extend_raw(pbase, ext->buffer, ext->used, repetitions);
}

XTR_API void
xtr_extend_from(xtr_t** const pbase, const char* const ext) // TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    const size_t ext_len = strlen(ext);
    xtr_extend_raw(pbase, (const uint8_t*) ext, ext_len, 1);
}

XTR_API void
xtr_extend_from_many(xtr_t** const pbase, const char* const ext, const size_t repetitions)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    const size_t ext_len = strlen(ext);
    xtr_extend_raw(pbase, (const uint8_t*)ext, ext_len, repetitions);
}

XTR_API void // TODO name it push?
xtr_append(xtr_t** const pbase, const char c) // TODO some error code
{
    if (pbase == NULL || *pbase == NULL) { return; }
    xtr_extend_raw(pbase, (const uint8_t*) &c, 1, 1);
}

XTR_API void
xtr_append_many(xtr_t** const pbase, const char c, const size_t repetitions)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL) { return; }
    xtr_extend_raw(pbase, (const uint8_t*) &c, 1, repetitions);
}

XTR_API xtr_t*
xtr_repeated(const xtr_t* const xtr, const size_t repetitions)
{
    if (xtr == NULL) { return NULL; }
    return xtr_from_bytes_repeat(xtr->buffer, xtr->used, repetitions);
}

XTR_API xtr_t*
xtr_concat(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL) { return NULL; }
    const size_t merged_len = a->used + b->used;
    if (merged_len < a->used || merged_len < b->used) { return NULL; } // Size overflow
    xtr_t* const merged = xtr_new(merged_len);
    if (merged == NULL) { return NULL; }
    memcpy(merged->buffer, a->buffer, a->used);
    memcpy(merged->buffer + a->used, b->buffer, b->used);
    set_used_and_terminator(merged, merged_len);
    return merged;
}
