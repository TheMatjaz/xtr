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

XTR_API xtr_t*
xtr_ensure(xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    return xtr_resize(xtr, XTR_MIN(xtr->used, len));
}

XTR_API xtr_t*
xtr_ensure_free(xtr_t** const pxtr, const size_t len)
{
    if (pxtr == NULL || *pxtr == NULL)
    {
        return NULL;
    }
    return xtr_resize_free(pxtr, XTR_MIN((*pxtr)->used, len));
}

XTR_API xtr_t*
xtr_resize(xtr_t* const xtr, const size_t new_capacity)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    if (new_capacity < xtr->used)
    {
        // Clear bytes at the end, but keep same allocation buffer
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
        zero_out(xtr->buffer + new_capacity, xtr->used - new_capacity);
#endif
        set_used_and_terminator(xtr, new_capacity);
        return xtr;
    }
    else
    {
        // Buffer needs to be expanded, reallocate
        return xtr_expanded(xtr, new_capacity);
    }
}

XTR_API xtr_t*
xtr_resize_free(xtr_t** const pxtr, const size_t new_length)
{
    if (pxtr == NULL)
    {
        return NULL;
    }
    xtr_t* const resized = xtr_resize(*pxtr, new_length);
    if (resized != *pxtr)
    {
        xtr_free(pxtr);
    }
    return resized;
}

XTR_API xtr_t*
xtr_resize_free_double(xtr_t** const pxtr)  // TODO clarify difference with clone_with_capacity
{
    if (pxtr == NULL || *pxtr == NULL)
    {
        return NULL;
    }
    const size_t doublesize = (*pxtr)->used * 2U;
    if (doublesize < (*pxtr)->used)
    {
        return NULL;
    }  // Size overflow
    return xtr_expand(pxtr, doublesize);
}

XTR_API xtr_t*
xtr_compress_free(xtr_t** const pxtr)  // TODO rename to compact?
{
    if (pxtr == NULL || xtr_available(*pxtr) == 0U)
    {
        return NULL;
    }
    xtr_t* const compressed = xtr_clone(*pxtr);
    if (compressed == NULL)
    {
        return NULL;
    }
    xtr_free(pxtr);
    return compressed;
}
