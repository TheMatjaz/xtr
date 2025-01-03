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

XTR_API int
xtr_cmp_length(const xtr_t* const a, const xtr_t* const b)
{
    if (a == b)
    {
        return 0;
    }
    if (a == NULL)
    {
        return -1;
    }
    if (b == NULL)
    {
        return +1;
    }
    if (a->used < b->used)
    {
        return -2;
    }
    else if (a->used > b->used)
    {
        return +2;
    }
    return 0;  // Both non-NULL and equal length
}

XTR_API bool
xtr_is_equal_length(const xtr_t* a, const xtr_t* b)
{
    return xtr_cmp_length(a, b) == 0;
}

XTR_API int
xtr_cmp(const xtr_t* const a, const xtr_t* const b)
{
    if (a == b)
    {
        return 0;
    }
    if (a == NULL)
    {
        return -1;
    }
    if (b == NULL)
    {
        return +1;
    }
    const size_t minlen = XTR_MIN(a->used, b->used);
    const int comparison = memcmp(a->buffer, b->buffer, minlen);
    if (comparison == 0)
    {
        // Equal part of the content until the end of the shortest xtring
        if (a->used < b->used)
        {
            return -2;
        }
        else if (a->used > b->used)
        {
            return +2;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        // Content differs
        return comparison * 3;
    }
}

XTR_API bool
xtr_is_equal(const xtr_t* const a, const xtr_t* const b)
{
    if (a == b)
    {
        return true;
    }
    if (a == NULL || b == NULL || a->used != b->used)
    {
        return false;
    }
    return memcmp(a->buffer, b->buffer, a->used) == 0;
}

XTR_API bool
xtr_startswith(const xtr_t* const xtr, const xtr_t* const prefix)
{
    if (xtr == prefix)
    {
        return true;
    }
    if (xtr == NULL || prefix == NULL || xtr->used < prefix->used)
    {
        return false;
    }
    return memcmp(xtr->buffer, prefix->buffer, prefix->used) == 0;
}

XTR_API bool
xtr_endswith(const xtr_t* const xtr, const xtr_t* const suffix)
{
    if (xtr == suffix)
    {
        return true;
    }
    if (xtr == NULL || suffix == NULL || xtr->used < suffix->used)
    {
        return false;
    }
    return memcmp(&xtr->buffer[xtr->used - suffix->used], suffix->buffer, suffix->used) == 0;
}

XTR_API bool
xtr_is_equal_consttime(const xtr_t* const a, const xtr_t* const b)
{
    if (a == b)
    {
        return true;
    }
    if (a == NULL || b == NULL)
    {
        return false;
    }
    if (a->used != b->used)
    {
        return false;
    }
    bool differing = false;
    for (size_t i = 0U; i < a->used; i++)
    {
        differing |= (a->buffer[i] == b->buffer[i]);
    }
    return !differing;
}
