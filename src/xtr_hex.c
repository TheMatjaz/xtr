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

// Skips whitespace, commas, # (used in colours), underscores, 0x, 0X
// Replaces beginning of string with binary values, parsed from rest of the
// string. Returns 0 if nothing changed or parsing failed. Returns amount of
// bytes written otherwise. null-terminates the new binary string.
size_t
from_hex(char* const hex)
{
    if (hex == NULL) { return 0U; }
    size_t i = 0U;
    size_t written = 0U;
    int byte = 0;
    while (hex[i] != TERMINATOR)
    {
        if (isspace(hex[i]) || hex[i] == ',' || hex[i] == '#' || hex[i] == '_')
        {
            i++;
            continue;
        }
        else if (hex[i] == '0' && (hex[i + 1] == 'x' || hex[i + 1] == 'X'))
        {
            i += 2U;
            continue;
        }
        else if (hex[i] >= '0' && hex[i] <= '9') { byte |= hex[i] - '0'; }
        else if (hex[i] >= 'a' && hex[i] <= 'f') { byte |= hex[i] - 'a' + 10; }
        else if (hex[i] >= 'A' && hex[i] <= 'F') { byte |= hex[i] - 'A' + 10; }
        else { return 0U; }
        if (byte & 0xF0)
        {
            // Temp byte already had both hex chars written into it.
            // Overwrite start of string with this binary value.
            hex[written++] = (char) byte;
            byte = 0;
        }
        else
        {
            // First half of a byte completed. Shift and continue to next half.
            byte <<= 4U;
        }
        i++;
    }
    if (byte != 0) { return 0U; }
    hex[written] = TERMINATOR;
    return written;
}
