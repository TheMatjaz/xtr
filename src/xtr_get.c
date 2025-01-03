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

XTR_API XTR_INLINE size_t
xtr_length(const xtr_t* xtr)
{
    if (xtr == NULL)
    {
        return 0U;
    }
    return xtr->used;
}

XTR_API XTR_INLINE size_t
xtr_capacity(const xtr_t* const xtr)
{
    if (xtr == NULL)
    {
        return 0U;
    }
    return xtr->capacity;
}

XTR_API XTR_INLINE size_t
xtr_available(const xtr_t* const xtr)
{
    if (xtr == NULL)
    {
        return 0U;
    }
    return xtr->capacity - xtr->used;
}

XTR_API const char*
xtr_cstring(const xtr_t* const xtr)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    else
    {
        return (const char*) xtr->buffer;
    }
}

XTR_API const uint8_t*
xtr_bytes(const xtr_t* xtr)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    else
    {
        return (const uint8_t*) xtr->buffer;
    }
}

XTR_API const uint8_t*
xtr_last(const xtr_t* const xtr)
{
    if (xtr == NULL)
    {
        return NULL;
    }
    return (const uint8_t*) &xtr->buffer[xtr->used - 1];
}
