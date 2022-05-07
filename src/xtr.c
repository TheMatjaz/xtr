/**
 * @file
 * Xtring - Extendable strings for C
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

#include "xtr.h"

#define TERMINATOR_LEN 1U
#define TERMINATOR '\0'
#define XTR_MIN(a, b) ((a) <  (b) ? (a) : (b))
#define XTR_MAX(a, b) ((a) >= (b) ? (a) : (b))
#define SIZE_OVERFLOW 0U

// TODO auto garbage collection?
// Consider making it a smart-pointer-like struct: add a counter of references
// a the free function actually reduces the counter. On zero: it frees.
// A self-garbage-collected thing?
/** @internal Xtring private structure */
struct xtr
{
    /** Available bytes for content in str_buffer, before terminator. */
    size_t max_str_len;
    /** Occupied bytes with content in str_buffer, before terminator. */
    size_t used_str_len;
    /** Buffer with content long `max_str_len+1`, always NULL-terminated at
     * `str_buffer[used_str_len]` and at `str_buffer[max_str_len]`. */
    char str_buffer[1U];
};

#ifndef memmem

static char*
memmem(const void* haystack, const size_t haystack_len,
       const void* const needle, const size_t needle_len)
{
    // TODO consider boyer-moore search
    size_t remaining = haystack_len;
    const uint8_t* occurrence = haystack;
    while (remaining < needle_len)
    {
        occurrence = memchr(occurrence, *(uint8_t*) needle, remaining);
        if (memcmp(occurrence, needle, needle_len) == 0) { return (void*) occurrence; }
        occurrence++;
        remaining--;
    }
    return NULL;
}

#endif
/**
 * @internal
 * Updates the length of the string in the buffer, terminating both string
 * and buffer to help against buffer overruns.
 *
 * @param [in, out] xtr to shrink/expand
 * @param [in] used_len new length of the string, before terminator
 */
XTR_INLINE static void
set_used_str_len_and_terminator(xtr_t* const xtr, const size_t used_len)
{
    xtr->used_str_len = used_len;
    xtr->str_buffer[xtr->used_str_len] = TERMINATOR;
    xtr->str_buffer[xtr->max_str_len] = TERMINATOR;
}

/**
 * @internal
 * Updates the maximum string length and terminating the buffer to help against
 * buffer overruns.
 *
 * @param [in, out] xtr to shrink/expand
 * @param [in] max_len new length of the buffer, before terminator
 */
XTR_INLINE static void
set_max_str_len_and_terminator(xtr_t* const xtr, const size_t max_len)
{
    xtr->max_str_len = max_len;
    xtr->str_buffer[xtr->max_str_len] = TERMINATOR;
}

/**
 * @internal
 * `sizeof(struct xtr)`, given the wanted max string length the struct should
 * hold.
 *
 * @param [in] max_str_len
 * @return size of the xtr struct or #SIZE_OVERFLOW if an integer overflow of
 * #size_t occurred.
 */
XTR_INLINE static size_t
struct_size(const size_t max_str_len)
{
    const size_t size = sizeof(size_t) * 2U + max_str_len + TERMINATOR_LEN;
    if (size <= max_str_len) { return SIZE_OVERFLOW; } else { return size; }
}

/**
 * @internal
 * Securely sets the data to all-zeros.
 * @param [in] data location to erase
 * @param [in] len length of \p data in bytes
 */
XTR_INLINE static void
xtr_zero_out(void* const data, volatile const size_t len)
{
    memset(data, 0, len);
    // TODO use something the compiler can't optimise away
    // Libsodium has good ideas
}

XTR_API xtr_t*
xtr_new_ensure(size_t max_len)
{
    const size_t to_allocate = struct_size(max_len);
    if (to_allocate == SIZE_OVERFLOW) { return NULL; }
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_t* const new = calloc(to_allocate, 1U);
#else
    xtr_t* const new = malloc(to_allocate);
#endif
    if (new == NULL) { return NULL; }
    set_max_str_len_and_terminator(new, max_len);
    set_used_str_len_and_terminator(new, 0U);
    return new;
}

XTR_API void
xtr_free(xtr_t** const pxtr)
{
    if (pxtr != NULL && *pxtr != NULL)
    {
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out((*pxtr)->str_buffer, (*pxtr)->max_str_len);
#endif
        free((*pxtr));
        *pxtr = NULL;  // Clear outside reference to avoid use-after-free
    }
}

XTR_API xtr_t*
xtr_new_empty(void)
{
    return xtr_new_ensure(0);
}

XTR_API xtr_t*
xtr_new_from_c(const char* const str) // TODO what about uint8_t arrays?
{
    if (str == NULL) { return NULL; }
    const size_t len = strlen(str);
    xtr_t* const new = xtr_new_ensure(len);
    if (new == NULL) { return NULL; }
    memcpy(new->str_buffer, str, len);
    set_used_str_len_and_terminator(new, len);
    return new;
}

XTR_API xtr_t*
xtr_new_from_c_ensure(const char* str, size_t at_least)
{
    if (str == NULL) { return xtr_new_ensure(at_least); }
    const size_t str_len = strlen(str);
    const size_t ensure_len = XTR_MAX(str_len, at_least);
    xtr_t* const new = xtr_new_ensure(ensure_len);
    if (new == NULL) { return NULL; }
    memcpy(new->str_buffer, str, str_len);
    set_used_str_len_and_terminator(new, str_len);
    return new;
}

XTR_API xtr_t*
xtr_new_clone(const xtr_t* const xtr)
{
    if (xtr == NULL) { return NULL; }
    const size_t allocation_size = struct_size(xtr->max_str_len);
    xtr_t* const new = malloc(allocation_size);
    if (new == NULL) { return NULL; }
    memcpy(new, xtr, allocation_size);
    return new;
}

XTR_API xtr_t*
xtr_new_clone_ensure(const xtr_t* const xtr, const size_t max_len)
{
    if (xtr == NULL) { return NULL; }
    const size_t xlen = xtr_len(xtr);
    xtr_t* const new = xtr_new_ensure(XTR_MAX(xlen, max_len));
    if (new == NULL) { return NULL; }
    memcpy(new, xtr, xlen);
    return new;
}

XTR_API xtr_t*
xtr_new_fill(const char c, const size_t len)
{
    xtr_t* const new = xtr_new_ensure(len);
    if (new == NULL) { return NULL; }
    memset(new->str_buffer, c, len);
    new->used_str_len = len;
    return new;
}

static xtr_t*
xtr_repeat_raw(const char* const part, const size_t repetitions, const size_t part_len)
{
    const size_t total_len = part_len * repetitions;
    if (total_len < part_len) { return NULL; }// TODO are all overflows cases handled correctly?
    xtr_t* const new = xtr_new_ensure(total_len);
    if (new == NULL) { return NULL; }
    for (size_t i = 0; i < repetitions; i++)
    {
        memcpy(&new->str_buffer[i * part_len], part, part_len);
    }
    set_used_str_len_and_terminator(new, total_len);
    return new;
}

XTR_API xtr_t*
xtr_new_repeat(const char* const str, const size_t repetitions)
{
    if (str == NULL) { return NULL; }
    const size_t part_len = strlen(str);
    return xtr_repeat_raw(str, repetitions, part_len);
}

XTR_API xtr_t*
xtr_repeat(const xtr_t* const xtr, const size_t repetitions)
{
    if (xtr == NULL) { return NULL; }
    return xtr_repeat_raw(xtr->str_buffer, repetitions, xtr->used_str_len);
}

XTR_API XTR_INLINE size_t
xtr_len(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->used_str_len;
}

XTR_API XTR_INLINE size_t
xtr_maxlen(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->max_str_len;
}

XTR_API XTR_INLINE size_t
xtr_available(const xtr_t* const xtr)
{
    if (xtr == NULL) { return 0U; }
    return xtr->max_str_len - xtr->used_str_len;
}

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
    for (size_t i = 0U; i < a->used_str_len; i += sizeof(uint64_t))
    {
        combined |= a->str_buffer[i];
    }
    return combined != 0;
}

XTR_API xtr_t*
xtr_merge(const xtr_t* const a, const xtr_t* const b)
{
    if (a == NULL || b == NULL) { return NULL; }
    const size_t merged_len = a->used_str_len + b->used_str_len;
    // Check for size_t overflowq
    if (merged_len < a->used_str_len || merged_len < b->used_str_len) { return NULL; }
    xtr_t* const merged = xtr_new_ensure(merged_len);
    if (merged == NULL) { return NULL; }
    memcpy(merged->str_buffer, a->str_buffer, a->used_str_len);
    memcpy(merged->str_buffer + a->used_str_len, b->str_buffer, b->used_str_len);
    set_used_str_len_and_terminator(merged, merged_len);
    return merged;
}

XTR_API xtr_t*
xtr_resize(xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL) { return NULL; }
    if (xtr->used_str_len > len)
    {
        // Clear bytes at the end, do keep same allocation str_buffer
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out(xtr->str_buffer + len, xtr->used_str_len - len);
#endif
        set_used_str_len_and_terminator(xtr, len);
        return xtr;
    }
    else
    {
        // Buffer needs to be expanded, reallocate
        const size_t to_allocate = struct_size(len);
        if (to_allocate == SIZE_OVERFLOW) { return NULL; }
        xtr_t* const new = realloc(xtr, to_allocate);
        if (new == NULL) { return NULL; }
        set_max_str_len_and_terminator(new, len);
        return new;
    }
}

XTR_API xtr_t*
xtr_resize_free(xtr_t** const pxtr, const size_t len)
{
    // TODO copy *pxtr into a local pointer, if the outer changes in the meantime
    if (pxtr == NULL || *pxtr == NULL) { return NULL; }
    if ((*pxtr)->used_str_len > len)
    {
        // Clear bytes at the end, do keep same allocation str_buffer
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out((*pxtr)->str_buffer + len, (*pxtr)->used_str_len - len);
#endif
        set_used_str_len_and_terminator((*pxtr), len);
        return (*pxtr);
    }
    else
    {
        // Buffer needs to be expanded, reallocate
        const size_t to_allocate = struct_size(len);
        if (to_allocate == SIZE_OVERFLOW) { return NULL; }
        xtr_t* const new = realloc((*pxtr), to_allocate);
        if (new == NULL) { return NULL; }
        if (new != (*pxtr))
        {
            xtr_free(pxtr);
            *pxtr = new;
        }
        set_max_str_len_and_terminator(new, len);
        return new;
    }
}

XTR_API xtr_t*
xtr_compress_free(xtr_t** const pxtr)
{
    if (pxtr == NULL || xtr_available(*pxtr) > 0U) { return NULL; }
    xtr_t* const compressed = xtr_new_clone(*pxtr);
    if (compressed == NULL) { return NULL; }
    xtr_free(pxtr);
    return compressed;
}

XTR_API const char*
xtr_cstring(const xtr_t* const xtr)
{
    if (xtr == NULL) { return NULL; }
    else { return xtr->str_buffer; }
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

XTR_API const char*
xtr_find(const xtr_t* const xtr, const xtr_t* const pattern)
{
    return xtr_find_in(xtr, pattern, 0U, xtr->used_str_len);
}

XTR_API const char*
xtr_find_from(const xtr_t* const xtr, const xtr_t* const pattern, const size_t start)
{
    return xtr_find_in(xtr, pattern, start, xtr->used_str_len);
}

XTR_API const char*
xtr_find_in(const xtr_t* const xtr, const xtr_t* const pattern,
            const size_t start, const size_t end)
{
    // Consider returning const char*
    if (xtr_is_empty(xtr) || xtr_is_empty(pattern)
        || start >= xtr->used_str_len || end >= xtr->used_str_len) { return NULL; }
    const char* const location = memmem(xtr->str_buffer + start,
                                        end - start, pattern->str_buffer, pattern->used_str_len);
    return location;
}

XTR_API void
xtr_clear(xtr_t* const xtr)
{
    if (xtr != NULL)
    {
#if (defined(XTR_SAFE) && XTR_SAFE)
        xtr_zero_out(xtr->str_buffer, xtr->max_str_len);
#endif
        set_used_str_len_and_terminator(xtr, 0U);
    }
}

XTR_API void
xtr_rtrim(xtr_t* const xtr, const char* const chars)
{
    if (xtr_is_empty(xtr)) { return; }
    size_t last = xtr->used_str_len - 1U; // Shifting index of last character, moving towards 0.
    // Stop when non-space found or when `last` loops around (`0--`)
    // thus the entire str_buffer was already explored.
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (last < xtr->used_str_len && isspace(xtr->str_buffer[last])) { last--; }
    }
    else
    {
        while (last < xtr->used_str_len && strchr(chars, xtr->str_buffer[last])) { last--; }
    }
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_zero_out(&xtr->str_buffer[last], (xtr->used_str_len - 1U) - last);
#endif
    set_used_str_len_and_terminator(xtr, last);
}

XTR_API void // O(n) operation!
xtr_ltrim(xtr_t* const xtr, const char* const chars)
{
    if (xtr_is_empty(xtr)) { return; }
    size_t first = 0U; // Shifting index of first character, moving towards end.
    // Stop when non-space found or when `first` hits string end,
    // thus the entire str_buffer was already explored.
    if (chars == NULL || *chars == TERMINATOR)
    {
        // No characters to trim given: trimming whitespace.
        while (first < xtr->used_str_len && isspace(xtr->str_buffer[first])) { first++; }
    }
    else
    {
        while (first < xtr->used_str_len && strchr(chars, xtr->str_buffer[first])) { first++; }
    }
    memmove(xtr->str_buffer, &xtr->str_buffer[first], xtr->used_str_len - first);
#if (defined(XTR_SAFE) && XTR_SAFE)
    xtr_zero_out(&xtr->str_buffer[xtr->used_str_len - first],
                 first); // first == amount of discarded
#endif
    set_used_str_len_and_terminator(xtr, xtr->used_str_len - first);
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

XTR_API xtr_t*
xtr_pop(xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL) { return NULL; }
    const size_t to_pop = XTR_MIN(len, xtr->used_str_len);
    xtr_t* const popped = xtr_new_from_c_ensure(&xtr->str_buffer[xtr->used_str_len - to_pop],
                                                to_pop);
    // FIXME returning the wrong thing
    return xtr_resize(xtr, xtr->used_str_len - to_pop);
}

static void
utf8_encode(uint8_t* encoded, const uint32_t codepoint)
{
    if (codepoint <= 0x7FU)
    {
        // 7 bits, encoded as 0xxx_xxxx
        encoded[0] = (uint8_t) codepoint;
    }
    else if (codepoint <= 0x3FFU)
    {
        // 11 bits, encoded as 110x_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xC0U | ((codepoint >> 6U) & 0x1FU));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0xFFFFU)
    {
        // 16 bits, encoded as 1110_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xE0U | ((codepoint >> 12U) & 0x0FU));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0x1FFFFFU)
    {
        // 21 bits, encoded as 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xF0U | ((codepoint >> 18U) & 0x07U));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[3] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0x3FFFFFFU)
    {
        // 26 bits, encoded as 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xF8U | ((codepoint >> 24U) & 0x03U));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 18U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[3] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[4] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0x7FFFFFFFU)
    {
        // 31 bits, encoded as 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xFCU | ((codepoint >> 30U) & 0x01U));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 24U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 18U) & 0x3FU));
        encoded[3] = (uint8_t) (0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[4] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[5] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else
    {
        // TODO unsupported codepoint
    }
}

#define UNICODE_REPLACEMENT_CHAR 0xFFFDU

static uint32_t
utf8_decode(const uint8_t* encoded)
{
    while ((*encoded & 0xC0U) == 0x80U)
    {
        // Non-first byte of UTF-8 encoding: 10xx_xxxx
        // Skip until a first byte is found (self-sychronisation).
        encoded++;
    }
    if ((*encoded & 0xC0U) == 0xC0U)
    {
        // First byte of UTF-8 encoding: 11xx_xxxx
        // Search for most significant zero, to decode how many bytes follow.
        register uint_fast8_t bytes_following = 1U;
        register uint8_t mask = 0x20U;
        while (*encoded & mask)
        {
            mask >>= 1U;
            bytes_following++;
        }
        // mask-1 covers all useful bits of the first byte byte
        uint32_t codepoint = encoded[0] & (mask - 1U);
        while (bytes_following--)
        {
            encoded++;
            if ((*encoded & 0xC0U) == 0x80U)
            {
                // Correct non-first byte of UTF-8 encoding: 10xx_xxxx
                codepoint <<= 6U;
                codepoint |= *encoded & 0x3FU;
            }
            else
            {
                // Unexpected byte after first one.
                codepoint = UNICODE_REPLACEMENT_CHAR; // TODO yield
                break;
            }
        }
        return codepoint;
    }
    else
    {
        // First and only byte of UTF-8 encoding: ASCII character, 0xxx_xxxx
        return *encoded;
    }
}

static void
xtr_extend_raw(xtr_t** const pbase, const char* const part,
               const size_t part_len, const size_t repetitions)
{
    const size_t total_len = ((*pbase)->used_str_len + part_len) * repetitions;
    // Check for size_t overflow
    if (total_len < (*pbase)->used_str_len || total_len < part_len) { return; }
    // TODO not all overflow  cases are checkde
    xtr_t* const resized = xtr_resize_free(pbase, total_len);
    if (resized == NULL) { return; }
    for (size_t i = 0U; i < repetitions; i++)
    {
        memcpy(&resized->str_buffer[i * part_len], part, part_len);
    }
    set_used_str_len_and_terminator(resized, total_len);
}

XTR_API void
xtr_extend(xtr_t** const pbase, const xtr_t* ext)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    xtr_extend_raw(pbase, ext->str_buffer, ext->used_str_len, 1);
}

XTR_API void
xtr_extend_many(xtr_t** const pbase, const xtr_t* ext, const size_t repetitions)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    xtr_extend_raw(pbase, ext->str_buffer, ext->used_str_len, repetitions);
}

XTR_API void
xtr_extend_from(xtr_t** const pbase, const char* const ext) // TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    const size_t ext_len = strlen(ext);
    xtr_extend_raw(pbase, ext, ext_len, 1);
}

XTR_API void
xtr_extend_from_many(xtr_t** const pbase, const char* const ext, const size_t repetitions)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL || ext == NULL) { return; }
    const size_t ext_len = strlen(ext);
    xtr_extend_raw(pbase, ext, ext_len, repetitions);
}

XTR_API void
xtr_append(xtr_t** const pbase, const char c) // TODO some error code
{
    if (pbase == NULL || *pbase == NULL) { return; }
    xtr_extend_raw(pbase, &c, 1, 1);
}

XTR_API void
xtr_append_many(xtr_t** const pbase, const char c, const size_t repetitions)
// TODO some error code
{
    if (pbase == NULL || *pbase == NULL) { return; }
    xtr_extend_raw(pbase, &c, 1, repetitions);
}

XTR_API bool
xtr_contains(const xtr_t* const xtr, const xtr_t* const pattern)
{
    return xtr_find_in(xtr, pattern, 0U, xtr->used_str_len) != NULL;
}

XTR_API char
xtr_last(const xtr_t* const xtr)
{
    if (xtr_is_empty(xtr)) { return TERMINATOR; }
    else { return xtr->str_buffer[xtr->used_str_len - 1U]; }
}

XTR_API xtr_t*
xtr_reversed(const xtr_t* const xtr)
{
    if (xtr == NULL) { return NULL; }
    xtr_t* const reversed = xtr_new_ensure(xtr->used_str_len);
    if (reversed == NULL) { return NULL; }
    for (size_t s = 0U, e = xtr->used_str_len - 1U; s < xtr->used_str_len; s++, e--)
    {
        reversed->str_buffer[s] = xtr->str_buffer[e];
    }
    set_used_str_len_and_terminator(reversed, xtr->used_str_len);
    return reversed;
}

XTR_API void
xtr_reverse(xtr_t* const xtr)
{
    if (xtr == NULL) { return; } // TODO errcodes
    char temp;
    for (size_t s = 0U, e = xtr->used_str_len - 1U; s < xtr->used_str_len; s++, e--)
    {
        temp = xtr->str_buffer[s];
        xtr->str_buffer[s] = xtr->str_buffer[e];
        xtr->str_buffer[e] = temp;
    }
}

XTR_API size_t
xtr_occurrences(const xtr_t* const xtr, const xtr_t* const pattern)
{
    if (xtr_is_empty(xtr) || xtr_is_empty(pattern)) { return 0U; }
    size_t count = 0U;
    const char* prev_occurrence = xtr->str_buffer;
    const char* this_occurrence = NULL;
    size_t remaining_len = xtr->used_str_len;
    do
    {
        this_occurrence = memmem(prev_occurrence, remaining_len,
                                 pattern->str_buffer, pattern->used_str_len);
        if (this_occurrence != NULL)
        {
            count++;
            // TODO use ptrdiff
            remaining_len -= (this_occurrence - prev_occurrence - pattern->used_str_len);
            prev_occurrence = this_occurrence + pattern->used_str_len;
        }
        else { break; }
    }
    while (true);
    return count;
}

XTR_API xtr_t**
xtr_split_at(const xtr_t* const xtr, const xtr_t* const pattern) // TODO return amount of parts!
{
    if (xtr_is_empty(xtr) || xtr_is_empty(pattern)) { return NULL; }
    const size_t occurrences_amount = xtr_occurrences(xtr, pattern);
    const size_t parts_amount = occurrences_amount + 1U;
    if (parts_amount <= occurrences_amount) { return NULL; }
    xtr_t** const parts = calloc(parts_amount, sizeof(xtr_t*));
    const char* prev_occurrence = xtr->str_buffer;
    const char* this_occurrence = NULL;
    xtr_t* part = NULL;
    size_t part_len = 0U;
    size_t remaining_len = xtr->used_str_len;
    size_t i = 0;
    do
    {
        // TODO find a way to combine the iterator function into one
        // from this function and occurences() and find()
        this_occurrence = memmem(prev_occurrence, remaining_len,
                                 pattern->str_buffer, pattern->used_str_len);
        if (this_occurrence != NULL)
        {
            part_len = this_occurrence - prev_occurrence - pattern->used_str_len;
            part = xtr_new_ensure(part_len);
            if (part != NULL)
            {
                parts[i++] = part;
            }
            else
            {
                // Free existing parts
                for (size_t p = 0; p < parts_amount; p++)
                {
                    xtr_free(&parts[p]);
                }
                free(parts);
                return NULL;
            }
            remaining_len -= this_occurrence - prev_occurrence;
            prev_occurrence = this_occurrence + pattern->used_str_len;
        }
        else { break; }
    }
    while (true);
    return parts;
}

XTR_API xtr_t**
xtr_split_every(const xtr_t* const xtr, const size_t part_len) // TODO return amount of parts
{
    if (xtr_is_empty(xtr) || part_len == 0) { return NULL; }
    // TODO
    return NULL;
}

XTR_API xtr_t**
xtr_split_into(const xtr_t* const xtr, const size_t parts_amount)
{
    if (xtr_is_empty(xtr) || parts_amount == 0) { return NULL; }
    // TODO
    return NULL;
}
