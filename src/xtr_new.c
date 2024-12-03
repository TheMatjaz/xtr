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

xtr_t*
xtr_alloc(const size_t used, const size_t capacity)
{
    if (used > capacity)
    {
        return NULL;
    }
    const size_t to_allocate = sizeof_struct_xtr(capacity);
    if (to_allocate == SIZE_OVERFLOW)
    {
        return NULL;
    }
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
    xtr_t* const new = XTR_CALLOC(to_allocate, sizeof(uint8_t));
#else
    xtr_t* const new = XTR_MALLOC(to_allocate);
#endif
    if (new == NULL)
    {
        return NULL;
    }
    set_capacity_and_terminator(new, capacity);
    set_used_and_terminator(new, 0U);
    return new;
}

XTR_API xtr_t*
xtr_new(size_t capacity)
{
    return xtr_alloc(0U, capacity);
}

XTR_API XTR_INLINE xtr_t*
xtr_new_empty(void)
{
    return xtr_alloc(0U, 0U);
}

XTR_API void
xtr_free(xtr_t** const pxtr)
{
    if (pxtr != NULL && *pxtr != NULL)
    {
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
        zero_out((*pxtr)->buffer, (*pxtr)->capacity);
#endif
        XTR_FREE(*pxtr);
        *pxtr = NULL;  // Clear outside reference to avoid use-after-free
    }
}
