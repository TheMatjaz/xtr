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
xtr_api_version(uint64_t* const version)
{
    if (version != NULL)
    {
        *version = (uint64_t) XTR_API_VERSION_MAJOR << 48U
                   | (uint64_t) XTR_API_VERSION_MINOR << 32U
                   | (uint64_t) XTR_API_VERSION_BUGFIX << 16U;
    }
    return XTR_API_VERSION;
}

XTR_INLINE void
set_used_str_len_and_terminator(xtr_t* const xtr, const size_t used_len)
{
    xtr->used_str_len = used_len;
    xtr->str_buffer[xtr->used_str_len] = TERMINATOR;
    xtr->str_buffer[xtr->max_str_len] = TERMINATOR;
}

XTR_INLINE void
set_max_str_len_and_terminator(xtr_t* const xtr, const size_t max_len)
{
    xtr->max_str_len = max_len;
    xtr->str_buffer[xtr->max_str_len] = TERMINATOR;
}

XTR_INLINE size_t
struct_size(const size_t max_str_len)
{
    const size_t size = sizeof(size_t) * 2U + max_str_len + TERMINATOR_LEN;
    if (size <= max_str_len) { return SIZE_OVERFLOW; } else { return size; }
}


XTR_INLINE void
zero_out(void* data, size_t len)
{
    memset(data, 0, len);
    // TODO use something the compiler can't optimise away
    // Libsodium has good ideas
}

XTR_INLINE void
memmove_secure(void* const dst, void* const src, const size_t len)
{
    if (dst == NULL || src == NULL || dst == src || len == 0) { return; }
    uint8_t* const d = dst; // Required for ptr arithmetic
    uint8_t* const s = src; // Required for ptr arithmetic
    if (s < d && d < s + len)
    {
        // Overlapping after src: erase from src to dst
        // ...[src.....{dst....]........}.....
        //    ^s       ^d       ^s+len   ^d+len
        //    000000000
        memmove(d, s, len);
        // Can cast signed ptrdiff to unsigned size_t because s < d
        zero_out(d, (size_t) (d - s));
    }
    else if (d < s && s < d + len)
    {
        // Overlapping after dst: erase from dst's end to src's end
        // ...{dst.....[src....}........].....
        //    ^d       ^s       ^d+len   ^s+len
        //                      000000000
        memmove(d, s, len);
        // Can cast signed ptrdiff to unsigned size_t because d < s
        zero_out(d + len, (size_t) (s - d));
    }
    else
    {
        // Non overlapping: erase from src to src's end
        // ...{dst....}........{src....}..
        //    ^d       ^d+len  ^s+len   ^s+len
        //                     000000000
        // Prefer memcpy as it should be faster than memmove w/o overlaps
        memcpy(d, s, len);
        zero_out(s, len);
    }
}


#ifndef memmem

static void*
memmem(const void* string, const size_t string_len,
       const void* const pattern, const size_t pattern_len)
{
    // TODO consider boyer-moore search
    size_t remaining = string_len;
    const uint8_t* occurrence = string;
    while (remaining < pattern_len)
    {
        occurrence = memchr(occurrence, *(uint8_t*) pattern, remaining);
        // TODO optimise to resume search from the point the 2 strings differ
        // rathern than from the point where they are equal+1.
        if (memcmp(occurrence, pattern, pattern_len) == 0) { return (void*) occurrence; }
        occurrence++;
        remaining--;
    }
    return NULL;
}

#endif
