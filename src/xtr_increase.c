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

#include "xtr_internal.h"

XTR_API size_t
xtr_push_tail(xtr_t* const xtr, const xtr_t* const extension)
{
    if (xtr == NULL || extension == NULL || xtr_capacity(xtr) < extension->used)
    {
        return 0U;
    }
    memcpy(&xtr->buffer[xtr->used], extension->buffer, extension->used);
    set_used_and_terminator(xtr, xtr->used + extension->used);
    return extension->used;
}

XTR_API size_t
xtr_push_head(xtr_t* const xtr, const xtr_t* const extension)
{
    if (xtr == NULL || extension == NULL || xtr_capacity(xtr) < extension->used)
    {
        return 0U;
    }
    memmove(&xtr->buffer[extension->used], xtr->buffer, xtr->used);
    memcpy(xtr->buffer, extension->buffer, extension->used);
    set_used_and_terminator(xtr, xtr->used + extension->used);
    return extension->used;
}

XTR_API xtr_t*
xtr_extend_tail(xtr_t** const pxtr, const xtr_t* const extension)
{
    // TODO copy pointers locally for reentrancy
    if (pxtr == NULL || *pxtr == NULL || extension == NULL)
    {
        return NULL;
    }
    if (xtr_capacity(*pxtr) >= extension->used)
    {
        xtr_push_tail(*pxtr, extension);
    }
    else
    {
        xtr_t* const new = xtr_concat(*pxtr, extension);
        if (new == NULL)
        {
            return NULL;
        }
        xtr_free(pxtr);
        *pxtr = new;
    }
    return *pxtr;
}

XTR_API xtr_t*
xtr_extend_head(xtr_t** const pxtr, const xtr_t* const extension)
{
    if (pxtr == NULL || *pxtr == NULL || extension == NULL)
    {
        return NULL;
    }
    if (xtr_capacity(*pxtr) >= extension->used)
    {
        xtr_push_head(*pxtr, extension);
    }
    else
    {
        xtr_t* const new = xtr_concat(extension, *pxtr);
        if (new == NULL)
        {
            return NULL;
        }
        xtr_free(pxtr);
        *pxtr = new;
    }
    return *pxtr;
}

XTR_API xtr_t*
xtr_repeated(const xtr_t* const xtr, const size_t repetitions)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    return xtr_from_bytes_repeat(xtr->buffer, xtr->used, repetitions);
}

XTR_API xtr_t*
xtr_concat(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL)
    {
        return NULL;
    }
    const size_t merged_len = a->used + b->used;
    if (merged_len < a->used || merged_len < b->used)
    {
        return NULL;
    }  // Size overflow
    xtr_t* const merged = xtr_malloc(merged_len, merged_len);
    if (merged == NULL)
    {
        return NULL;
    }
    memcpy(merged->buffer, a->buffer, a->used);
    memcpy(merged->buffer + a->used, b->buffer, b->used);
    set_used_and_terminator(merged, merged_len);
    return merged;
}
