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
    if (xtr == NULL) { return NULL; }
    return xtr_resize(xtr, XTR_MIN(xtr->used, len));
}
XTR_API xtr_t*
xtr_ensure_free(xtr_t** const pxtr, const size_t len)
{
    if (pxtr == NULL || *pxtr == NULL) { return NULL; }
    return xtr_resize_free(pxtr, XTR_MIN((*pxtr)->used, len));
}

// TODO xtr_ensure: like resize, but does not truncate if too short
XTR_API xtr_t*
xtr_resize(xtr_t* const xtr, const size_t len)
{
    if (xtr == NULL) { return NULL; }
    if (xtr->used > len)
    {
        // Clear bytes at the end, do keep same allocation buffer
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
        zero_out(xtr->buffer + len, xtr->used - len);
#endif
        set_used_and_terminator(xtr, len);
        return xtr;
    }
    else
    {
        // Buffer needs to be expanded, reallocate
        const size_t to_allocate = sizeof_struct_xtr(len);
        if (to_allocate == SIZE_OVERFLOW) { return NULL; }
        xtr_t* const new = realloc(xtr, to_allocate);
        if (new == NULL) { return NULL; }
        set_capacity_and_terminator(new, len);
        return new;
    }
}

XTR_API xtr_t*
xtr_resize_free(xtr_t** const pxtr, const size_t len)
{
    // TODO copy *pxtr into a local pointer, if the outer changes in the meantime
    if (pxtr == NULL || *pxtr == NULL) { return NULL; }
    if ((*pxtr)->used > len)
    {
        // Clear bytes at the end, do keep same allocation buffer
#if (defined(XTR_CLEAR_HEAP) && XTR_CLEAR_HEAP)
        zero_out((*pxtr)->buffer + len, (*pxtr)->used - len);
#endif
        set_used_and_terminator((*pxtr), len);
        return (*pxtr);
    }
    else
    {
        // Buffer needs to be expanded, reallocate
        const size_t to_allocate = sizeof_struct_xtr(len);
        if (to_allocate == SIZE_OVERFLOW) { return NULL; }
        xtr_t* const new = realloc((*pxtr), to_allocate);
        if (new == NULL) { return NULL; }
        if (new != (*pxtr))
        {
            xtr_free(pxtr);
            *pxtr = new;
        }
        set_capacity_and_terminator(new, len);
        return new;
    }
}

XTR_API xtr_t*
xtr_compress_free(xtr_t** const pxtr)
{
    if (pxtr == NULL || xtr_available(*pxtr) > 0U) { return NULL; }
    xtr_t* const compressed = xtr_clone(*pxtr);
    if (compressed == NULL) { return NULL; }
    xtr_free(pxtr);
    return compressed;
}
