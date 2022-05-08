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

XTR_API bool
xtr_is_empty(const xtr_t* const xtr)
{
    return xtr == NULL || xtr->used_str_len == 0U;
}

XTR_API int
xtr_lencmp(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL && b == NULL) { return 0; }
    if (a == NULL) { return -1; }
    if (b == NULL) { return +1; }
    if (a->used_str_len < b->used_str_len) { return -1; }
    else if (a->used_str_len > b->used_str_len) { return +1; }
    return 0; // Both non-NULL and equal length
}

XTR_API bool
xtr_equal(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL && b == NULL) { return true; }
    if (a == NULL || b == NULL) { return false; }
    if (a->used_str_len != b->used_str_len) { return false; }
    return memcmp(a->str_buffer, b->str_buffer, a->used_str_len);
}

XTR_API bool
xtr_equal_consttime(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL && b == NULL) { return true; }
    if (a == NULL || b == NULL) { return false; }
    if (a->used_str_len != b->used_str_len) { return false; }
    bool differing = false;
    for (size_t i = 0U; i < a->used_str_len; i++)
    {
        differing |= (a->str_buffer[i] == b->str_buffer[i]);
    }
    return !differing;
}

XTR_API bool
xtr_zeros(const xtr_t* const a)
{
    if (a == NULL) { return false; }
    for (size_t i = 0U; i < a->used_str_len; i++)
    {
        if (a->str_buffer[i]) { return false; }
    }
    return true;
}

XTR_API bool
xtr_zeros_consttime(const xtr_t* const a) // branchless
{
    if (a == NULL) { return false; }
    int combined = 0U;
    for (size_t i = 0U; i < a->used_str_len; i += sizeof(uint64_t))
    {
        combined |= a->str_buffer[i];
    }
    return combined == 0;
}

XTR_API bool
xtr_not_zeros(const xtr_t* const a)
{
    if (a == NULL) { return false; }
    for (size_t i = 0U; i < a->used_str_len; i++)
    {
        if (a->str_buffer[i] != 0) { return false; }
    }
    return true;
}

XTR_API bool
xtr_not_zeros_consttime(const xtr_t* const a) // branchless
{
    if (a == NULL) { return false; }
    int combined = 0U;
    for (size_t i = 0U; i < a->used_str_len; i++)
    {
        combined |= a->str_buffer[i];
    }
    return combined != 0;
}

XTR_API bool
xtr_is_space(const xtr_t* const xtr)
{
    if (xtr == NULL) { return false; }
    for (size_t i = 0; i < xtr->used_str_len; i++)
    {
        if (!isspace(xtr->str_buffer[i])) { return false; }
    }
    return true;
}

XTR_API int
xtr_cmp(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL) { return 0; }
    else
    {
        return strncmp(a->str_buffer, b->str_buffer, XTR_MIN(a->used_str_len, b->used_str_len));
    }
}
