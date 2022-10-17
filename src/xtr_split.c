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

XTR_API xtr_t**
xtr_split_at(const xtr_t* const xtr, const xtr_t* const pattern) // TODO return amount of parts!
{
    if (xtr_is_empty(xtr) || xtr_is_empty(pattern)) { return NULL; }
    const size_t occurrences_amount = xtr_occurrences(xtr, pattern);
    const size_t parts_amount = occurrences_amount + 1U;
    if (parts_amount <= occurrences_amount) { return NULL; }
    xtr_t** const parts = calloc(parts_amount, sizeof(xtr_t*));
    const uint8_t* prev_occurrence = xtr->buffer;
    const uint8_t* this_occurrence = NULL;
    xtr_t* part = NULL;
    size_t part_len = 0U;
    size_t remaining_len = xtr->used;
    size_t i = 0;
    do
    {
        // TODO find a way to combine the iterator function into one
        // from this function and occurences() and find()
        this_occurrence = memmem(prev_occurrence, remaining_len,
                                 pattern->buffer, pattern->used);
        if (this_occurrence != NULL)
        {
            part_len = this_occurrence - prev_occurrence - pattern->used;
            part = xtr_new(part_len);
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
            prev_occurrence = this_occurrence + pattern->used;
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
