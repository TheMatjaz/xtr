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
xtr_from_hex(const char* hex, size_t len)
{
    if (hex == NULL && len != 0U) { return NULL; }
    if (len == XTR_UNKNOWN_STRLEN) { len = strlen(hex); }
    xtr_t* bin = xtr_new(len / 2U);
    size_t converted = 0U;
    int byte = 0;
    while (*hex != TERMINATOR)
    {
        // Skip formatting/beautifying non-hex characters
        if (isspace(*hex) || *hex == ',' || *hex == '#' || *hex == '_')
        {
            hex++;
            continue;
        }
            // Skip 0x or 0X
        else if (*hex == '0' && (*(hex + 1) == 'x' || *(hex + 1) == 'X'))
        {
            hex += 2U;
            continue;
        }
            // Process actual hex characters
        else if (*hex >= '0' && *hex <= '9') { byte |= *hex - '0'; }
        else if (*hex >= 'a' && *hex <= 'f') { byte |= *hex - 'a' + 10; }
        else if (*hex >= 'A' && *hex <= 'F') { byte |= *hex - 'A' + 10; }
        else
        {
            // Non-hex character found.
            xtr_free(&bin);
            return NULL;
        }
        if (byte & 0xF0)
        {
            // Temp byte already has both hex chars (bin nibbles) written into it.
            bin->buffer[converted++] = (uint8_t) byte;
            byte = 0U;
        }
        else
        {
            // First half of a byte completed. Shift and continue to next half.
            byte <<= 4U;
        }
        hex++;
    }
    if (byte != 0)
    {
        // Hex string is over but one unpaired hex character (bin nibble) was
        // found. No way to tell where it should belong.
        xtr_free(&bin);
        return NULL;
    }
    set_used_and_terminator(bin, converted);
    return bin;
}

static const uint8_t HEXCHARS_UPPER[] = "0123456789ABCDEF";
static const uint8_t HEXCHARS_LOWER[] = "0123456789abcdef";

XTR_API xtr_t*
xtr_to_hex(const xtr_t* const bin, const bool upper, const char* const separator)
{
    if (bin == NULL) { return NULL; }
    size_t sep_len = 0U;
    if (separator != NULL) { sep_len = strlen(separator); }
    xtr_t* hex = xtr_new(bin->used * (2U + sep_len));
    if (hex == NULL) { return NULL; }
    const uint8_t* hexchars;
    if (upper) { hexchars = HEXCHARS_UPPER; }
    else { hexchars = HEXCHARS_LOWER; }
    size_t hex_index = 0U;
    for (size_t bin_index = 0U; bin_index < bin->used; bin_index++)
    {
        // Encode a byte to two hex characters
        hex->buffer[hex_index++] = hexchars[bin->buffer[bin_index] >> 4U];
        hex->buffer[hex_index++] = hexchars[bin->buffer[bin_index] & 0x0FU];
        if (separator != NULL)
        {
            memcpy(&hex->buffer[hex_index], separator, sep_len);
            hex_index += sep_len;
        }
    }
    set_used_and_terminator(hex, bin->used * (2U + sep_len));
    return hex;
}
