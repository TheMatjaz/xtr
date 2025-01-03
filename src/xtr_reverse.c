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

XTR_API xtr_t*
xtr_reversed(const xtr_t* const xtr)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    xtr_t* const reversed = xtr_new(xtr->used);
    if (reversed == NULL)
    {
        return NULL;
    }
    for (size_t head = 0U, tail = xtr->used - 1U; head < tail; head++, tail--)
    {
        reversed->buffer[head] = xtr->buffer[tail];
    }
    set_used_and_terminator(reversed, xtr->used);
    return reversed;
}

XTR_API void
xtr_reverse(xtr_t* const xtr)
{
    if (xtr == NULL)
    {
        return;
    }  // TODO errcodes
    uint8_t temp;
    for (size_t head = 0U, tail = xtr->used - 1U; head < tail; head++, tail--)
    {
        temp = xtr->buffer[head];
        xtr->buffer[head] = xtr->buffer[tail];
        xtr->buffer[tail] = temp;
    }
}
