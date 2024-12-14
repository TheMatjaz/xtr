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

XTR_API const char*
xtr_api_version(uint32_t* const version)
{
    if (version != NULL)
    {
        *version = (((uint32_t) XTR_API_VERSION_MAJOR) << 24U) |
                   (((uint32_t) XTR_API_VERSION_MINOR) << 16U) |
                   (((uint32_t) XTR_API_VERSION_BUGFIX) << 8U);
    }
    return XTR_API_VERSION;
}

XTR_INLINE void
set_used_and_terminator(xtr_t* xtr, size_t used_len)
{
    xtr->used = used_len;
    xtr->buffer[xtr->used] = TERMINATOR;
    xtr->buffer[xtr->capacity] = TERMINATOR;
}

XTR_INLINE void
set_capacity_and_terminator(xtr_t* xtr, size_t capacity)
{
    xtr->capacity = capacity;
    xtr->buffer[xtr->capacity] = TERMINATOR;
}

XTR_INLINE size_t
sizeof_struct_xtr(size_t capacity)
{
    const size_t size = sizeof(size_t) * 2U + capacity + TERMINATOR_LEN;
    if (size <= capacity)
    {
        return SIZE_OVERFLOW;
    }
    else
    {
        return size;
    }
}

XTR_INLINE void
zero_out(void* data, size_t len)
{
    memset(data, 0, len);
    // TODO use something the compiler can't optimise away
    // Libsodium has good ideas
}

XTR_INLINE void
memmove_zero_out(void* const dst, void* const src, const size_t len)
{
    if (dst == NULL || src == NULL || dst == src || len == 0U)
    {
        return;
    }
    uint8_t* const d = dst;  // Required for ptr arithmetic
    uint8_t* const s = src;  // Required for ptr arithmetic
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

const void*
xtr_memmem(const void* haystack_vp,
           const size_t haystack_len,
           const void* const needle_vp,
           const size_t needle_len)
{
    // TODO consider boyer-moore search
    if (haystack_vp == NULL || needle_vp == NULL || haystack_len == 0U || needle_len == 0U ||
        haystack_len < needle_len)
    {
        return NULL;
    }
    if (haystack_vp == needle_vp)
    {
        return haystack_vp;
    }
    const uint8_t* haystack = (const uint8_t*) haystack_vp;
    // haystack_end = last possible address where the needle could still be.
    // If not found until this point, the rest of the haystack is too short to fit a needle.
    const uint8_t* const haystack_end = haystack + haystack_len - needle_len;
    const uint8_t* haystack_search = haystack;
    const uint8_t* const needle = (const uint8_t*) needle_vp;
    const uint8_t* const needle_end = needle + haystack_len;
    const uint8_t* needle_search = needle;
    if (needle_len == 1U)
    {
        return memchr(haystack, needle[0], haystack_len);
    }
    while (true)
    {
        haystack_search =
            memchr(haystack_search, needle[0], (size_t) (haystack_end - haystack_search));
        if (haystack_search == NULL)
        {
            return NULL;
        }
        // First byte is matching
        haystack_search++;
        needle_search = &needle[1];
        // Match the rest of the needle body against the haystack
        while (haystack_search < haystack_end)
        {
            if (*haystack_search == *needle_search)
            {
                // Another byte matching
                haystack_search++;
                needle_search++;
                if (needle_search == needle_end)
                {
                    // Full match found
                    return haystack_search - needle_len;
                }
            }
            else
            {
                // Mismatch. Resume haystack seach for first needle byte
                // starting from where the needle-body matching stopped.
                break;
            }
        }
    }
    // TODO if the search reaches index haystack->used-needle->used+1U, then the rest does not
    //  contain a needle anyhow
    return NULL;
}
