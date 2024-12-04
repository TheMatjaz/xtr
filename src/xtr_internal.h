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

#ifndef XTR_INTERNAL_H
#define XTR_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "xtr.h"

#define TERMINATOR_LEN 1U
#define TERMINATOR     '\0'
#define XTR_MIN(a, b)  ((a) < (b) ? (a) : (b))
#define XTR_MAX(a, b)  ((a) >= (b) ? (a) : (b))
#define SIZE_OVERFLOW  0U

/**
 * @internal Xtring opaque (private) structure.
 *
 * Single-allocation flexible array with buffer length and used amount of
 * said buffer. Does not require a second level of pointer indirection
 * because all the data is contiguous.
 *
 * The useful data portion is null-terminated for backwards compatibility with
 * C-strings. The end of the buffer (a byte *after* the last useful buffer
 * byte) contains an extra null-terminator just as an extra precaution for
 * memory safety.
 *
 * In short: the user does not have to worry about null-terminators,
 * memory management (other than freeing the xtrings after use), or
 * keeping track of the data length and buffer sizes.
 *
 *                                +-- content always null-terminated
 *                                |
 *                                |          +-- buffer always null-terminated
 *                                |          |
 *                                v          v
 *
 *         [capacity][used][abcde\0.........\0]
 *
 *                          \___/            used (5, excl. null terminator)
 *                               \_________/ available free space (11)
 *                          \______________/ capacity (16, excl. null term.)
 *
 */
struct xtr
{
    /** Total bytes for content in buffer (used + free), before terminator. */
    size_t capacity;
    /** Occupied bytes with content in buffer out of the capacity. */
    size_t used;
    /** Buffer with the actual data. Buffer size = `capacity+1`, where the +1
     * is for a null-terminator at the buffer's end to protect against string
     * reads out of bounds. The content is also always null-terminated,
     * thus the nulls are at `buffer[used]` and at `buffer[capacity]`.
     * C99 flexible array member <https://en.wikipedia.org/wiki/Flexible_array_member> */
    uint8_t buffer[1U];
};

/**
 * @internal
 * Updates the length of the string in the buffer, terminating both string
 * and buffer to help against buffer overruns.
 *
 * @param [in, out] xtr to shrink/expand
 * @param [in] used_len new length of the string, before terminator
 */
void
set_used_and_terminator(xtr_t* xtr, size_t used_len);

/**
 * @internal
 * Updates the maximum string length and terminating the buffer to help against
 * buffer overruns.
 *
 * @param [in, out] xtr to shrink/expand
 * @param [in] capacity new length of the buffer, before terminator
 */
void
set_capacity_and_terminator(xtr_t* xtr, size_t capacity);

/**
 * @internal
 * `sizeof(struct xtr)`, given the wanted max string length the struct should
 * hold, including meatadata and null-terminations. Used for memory allocations.
 *
 * @param [in] capacity
 * @return size of the xtr struct or #SIZE_OVERFLOW if an integer overflow of
 * #size_t occurred.
 */
size_t
sizeof_struct_xtr(size_t capacity);  // todo reneame to xtr_sizeof

/**
 * @internal
 * Allocates a new xtring structure, clearing only the last `capacity-used` bytes.
 *
 * @param [in] used amount of bytes to be overwritten, NOT initialised.
 * @param [in] capacity buffer size.
 * @return the new xtring or NULL in case of malloc failure or integer overflow
 *         or if `used` > `capacity`.
 */
xtr_t*
xtr_alloc(size_t used, size_t capacity);

/**
 * @internal
 * Securely sets the data to all-zeros.
 * @param [in] data location to erase
 * @param [in] len length of \p data in bytes
 */
void
zero_out(void* data, size_t len);  // todo rename to xtr_zero_out

/**
 * @internal
 * Performs a memmove() and then zeros-out the non-overlapping source region.
 *
 * Source and destination regions may overlap. Does nothing if either is NULL
 * or if they are the same pointer or if they are empty (zero length).
 * @param dst start of the destination region.
 * @param src start of the source region.
 * @param len amount of bytes to move.
 */
void
memmove_zero_out(void* dst, void* src, size_t len);

/**
 * @internal
 * Searches a multi-byte pattern in a larger binary array.
 *
 * This is a custom implementation of the memmem() function, which is part of the
 * GNU libc, but not of the standard libc. This is a naive implementation of the
 * substring search algorithm but still tries to add a few optimisations, like
 * avoiding scanning any haystack byte twice. It should be performant-enough
 * for small to medium-sized arrays.
 *
 * @param haystack larger array to search in
 * @param haystack_len amount of bytes of the haystack array
 * @param needle smaller array to search for (a.k.a. pattern, substring)
 * @param needle_len amount of bytes of the needle array
 * @return pointer to the start of the first needle occurrence in haystack.
 *         NULL if not found, if either array is NULL or has zero length,
 *         or if the needle is too large to fit in the haystack.
 *         Note: if the needle and the haystack are the same pointer, it returns
 *         that pointer ("the needle is equal to itself" scenario)
 */
void*
xtr_memmem(const void* haystack, size_t haystack_len, const void* needle, size_t needle_len);

#ifdef __cplusplus
}
#endif

#endif /* XTR_INTERNAL_H */
