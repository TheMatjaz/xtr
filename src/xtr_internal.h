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

#ifndef XTR_INTERNAL_H
#define XTR_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "xtr.h"

#define TERMINATOR_LEN 1U
#define TERMINATOR '\0'
#define XTR_MIN(a, b) ((a) <  (b) ? (a) : (b))
#define XTR_MAX(a, b) ((a) >= (b) ? (a) : (b))
#define SIZE_OVERFLOW 0U
#define XTR_MALLOC(len) malloc(len)
#define XTR_CALLOC(len) calloc((len), 1U)
#define XTR_REALLOC(ptr, len) realloc((ptr), (len))

// TODO auto garbage collection?
// Consider making it a smart-pointer-like struct: add a counter of references
// a the free function actually reduces the counter. On zero: it frees.
// A self-garbage-collected thing?
/**
 * @internal Xtring private structure.
 * Single-allocation flexible array with buffer length and used amount of
 * said buffer. Does not require a second level of pointer indirection
 * because all the data is contiguous.
 *
 * The useful data portion is null-terminated for backwards compatibility with
 * C-strings. The end of the buffer (a byte *after* the last useful buffer
 * byte) contains an extra null-terminator just as an extra precaution for
 * memory safety.
 *
 *             capacity (buffer size)
 *             __________________
 *            /                  \
 *           [abcde\0............. \0] buffer
 *            \___/\_____________/
 *           length       available
 *         (used space)  (free space)
 */
struct xtr
{
    /** Total bytes for content in buffer (used + free), before terminator. */
    // TODO implement version with uint8_t/uint16_t instead of size_t
    size_t capacity;
    /** Occupied bytes with content in buffer out of the capacity. */
    size_t used;
    /** Buffer with content long `max_str_len+1`, always NULL-terminated at
     * `buffer[used_str_len]` and at `buffer[max_str_len]`.
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
 * hold, used for memory allocations.
 *
 * @param [in] max_str_len
 * @return size of the xtr struct or #SIZE_OVERFLOW if an integer overflow of
 * #size_t occurred.
 */
size_t
sizeof_struct_xtr(size_t max_str_len);

/**
 * @internal
 * Securely sets the data to all-zeros.
 * @param [in] data location to erase
 * @param [in] len length of \p data in bytes
 */
void
zero_out(void* data, size_t len);

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


#ifdef __cplusplus
}
#endif

#endif  /* XTR_INTERNAL_H */
