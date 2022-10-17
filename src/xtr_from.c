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

#define NO_REPETITIONS 1U
#define NO_EXTRA_CAPACITY 0U

XTR_API xtr_t*
xtr_from_str(const char* const str)
{
    return xtr_from_str_repeat_capac(str, NO_REPETITIONS, NO_EXTRA_CAPACITY);
}

XTR_API xtr_t*
xtr_from_str_capac(const char* str, size_t at_least)
{
    return xtr_from_str_repeat_capac(str, NO_REPETITIONS, at_least);
}

XTR_API xtr_t*
xtr_from_str_repeat(const char* str, size_t repetitions)
{
    return xtr_from_str_repeat_capac(str, repetitions, NO_EXTRA_CAPACITY);
}

XTR_API xtr_t*
xtr_from_str_repeat_capac(const char* str,
                          size_t repetitions,
                          size_t at_least)
{
    size_t len = 0U;
    if (str != NULL) { len = strlen(str); }
    return xtr_from_bytes_repeat_capac((const uint8_t*) str, len, repetitions, at_least);
}

XTR_API xtr_t*
xtr_from_bytes(const uint8_t* array,
               size_t array_len)
{
    return xtr_from_bytes_repeat_capac(array, array_len, NO_REPETITIONS,
                                       NO_EXTRA_CAPACITY);
}

XTR_API xtr_t*
xtr_from_bytes_capac(const uint8_t* array,
                     size_t array_len,
                     size_t at_least)
{
    return xtr_from_bytes_repeat_capac(array, array_len, NO_REPETITIONS, at_least);
}

XTR_API xtr_t*
xtr_from_bytes_repeat(const uint8_t* array,
                      size_t array_len,
                      size_t repetitions)
{
    return xtr_from_bytes_repeat_capac(array, array_len, repetitions, NO_EXTRA_CAPACITY);
}

XTR_API xtr_t*
xtr_from_bytes_repeat_capac(const uint8_t* array,
                            size_t array_len,
                            size_t repetitions,
                            size_t at_least)
{
    if (array == NULL && array_len != 0U) { return NULL; }
    if (repetitions == 0U) { return xtr_new(at_least); }
    const size_t total_len = repetitions * array_len;
    if (total_len < array_len) { return NULL; } // Integer overflow
    xtr_t* const new = xtr_new(XTR_MAX(total_len, at_least));
    if (new == NULL) { return NULL; }
    if (array == NULL) { return new; } // Don't risk passing NULL to memcpy
    for (size_t i = 0U; i < repetitions; i++)
    {
        memcpy(&new->buffer[i * array_len], array, array_len);
    }
    set_used_and_terminator(new, total_len);
    return new;
}

XTR_API xtr_t*
xtr_from_byte(const uint8_t byte)
{
    return xtr_from_byte_repeat_capac(byte, NO_REPETITIONS, NO_EXTRA_CAPACITY);
}

XTR_API xtr_t*
xtr_from_byte_repeat(uint8_t byte, size_t repetitions)
{
    return xtr_from_byte_repeat_capac(byte, repetitions, NO_EXTRA_CAPACITY);
}

XTR_API xtr_t*
xtr_from_byte_repeat_capac(uint8_t byte, size_t len,
                           size_t at_least)
{
    xtr_t* const new = xtr_new(XTR_MAX(len, at_least));
    if (new == NULL) { return NULL; }
    memset(new->buffer, byte, len);
    set_used_and_terminator(new, len);
    return new;
}

XTR_API xtr_t*
xtr_zeros(size_t len)
{
    xtr_t* const new = xtr_new(len);
    if (new == NULL) { return NULL; }
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
    // Uses calloc already, no need to clear
#else
    memset(new->buffer, 0, len);
#endif
    set_used_and_terminator(new, len);
    return new;
}
