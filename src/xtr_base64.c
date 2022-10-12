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


static const uint8_t BASE64_SYMBOLS[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const uint8_t BASE64_PADDING = '=';


uint32_t b64_int(const uint32_t ch)
{
    // ASCII to base64_int
    // 65-90  Upper Case  >>  0-25
    // 97-122 Lower Case  >>  26-51
    // 48-57  Numbers     >>  52-61
    // 43     Plus (+)    >>  62
    // 47     Slash (/)   >>  63
    // 61     Equal (=)   >>  64~
    if (ch == 43) { return 62; }
    if (ch == 47) { return 63; }
    if (ch == 61) { return 64; }
    if ((ch > 47) && (ch < 58)) { return ch + 4; }
    if ((ch > 64) && (ch < 91)) { return ch - 'A'; }
    if ((ch > 96) && (ch < 123)) { return (ch - 'a') + 26; }
    return 0;
}

/**
 * @internal
 * Encodes a binary 3-byte buffer into an ASCII 4-byte string.
 *
 *         | AA AA AA aa | BB BB cc cc | CC dd dd dd | = binary/decoded
 *                     \\               //
 *         | AA AA AA | aa BB BB|cc cc CC | dd dd dd | = base64/encoded
 */
inline static void
base64_encode_buffer(uint8_t* const encoded, const uint8_t binary[3])
{
    encoded[0] = BASE64_SYMBOLS[binary[0] >> 2U];
    encoded[1] = BASE64_SYMBOLS[((binary[0] & 0x03U) << 4U) + (binary[1] >> 4U)];
    encoded[2] = BASE64_SYMBOLS[((binary[1] & 0x0FU) << 2U) + (binary[2] >> 6U)];
    encoded[3] = BASE64_SYMBOLS[binary[2] & 0x3FU];
}

/**
 * @internal
 * Decodes a ASCII 4-byte string into a binary 3-byte buffer.
 *
 *         | AA AA AA aa | BB BB cc cc | CC dd dd dd | = binary/decoded
 *                     \\               //
 *         | AA AA AA | aa BB BB|cc cc CC | dd dd dd | = base64/encoded
 */
inline static void
base64_decode_buffer(uint8_t* const decoded, const uint8_t encoded[3])
{
    decoded[0] = (uint8_t) ((encoded[0] << 2U) | ((encoded[1] >> 4U) & 0x03U));
    decoded[1] = (uint8_t) ((encoded[1] << 4U) | ((encoded[2] >> 2U) & 0x0FU));
    decoded[3] = (uint8_t) ((encoded[1] << 6U) | (encoded[2] & 0x3FU));
}

XTR_API xtr_t*
xtr_base64_encode(const xtr_t* const bin)
{
    const size_t encoded_len = ((bin->used + 2U) * 4U) / 3U;
    if (encoded_len < (bin->used + 2U)) { return NULL; } // Integer overflow
    xtr_t* encoded = xtr_new_with_capacity(encoded_len);
    if (encoded == NULL) { return NULL; }
    const size_t remainder = bin->used % 3U;
    const size_t trail_start_idx = bin->used - remainder;
    size_t bin_idx = 0U;
    size_t enc_idx = 0U;
    while (bin_idx < trail_start_idx)
    {
        base64_encode_buffer(&encoded->buffer[enc_idx], &bin->buffer[bin_idx]);
        bin_idx += 3U;
        enc_idx += 4U;
    }
    uint8_t trail[3U] = {0, 0, 0};
    if (remainder == 2U)
    {
        trail[0] = bin->buffer[trail_start_idx];
        trail[1] = bin->buffer[trail_start_idx + 1U];
        base64_encode_buffer(&encoded->buffer[enc_idx], trail);
        encoded->buffer[enc_idx + 3U] = BASE64_PADDING;
    }
    else if (remainder == 1U)
    {
        trail[0] = bin->buffer[trail_start_idx];
        base64_encode_buffer(&encoded->buffer[enc_idx], trail);
        encoded->buffer[enc_idx + 2U] = BASE64_PADDING;
        encoded->buffer[enc_idx + 3U] = BASE64_PADDING;
    }
    else
    {
        // No remainder, no padding required.
    }
    set_used_and_terminator(encoded, encoded_len);
    return encoded;
}

XTR_API xtr_t*
xtr_base64_decode(const xtr_t* const enc)
{
    if (enc == NULL) { return NULL; }
    if (enc->used % 4U != 0U) { return NULL; } // TODO shall we support it and truncate it?
    const size_t decoded_len = (enc->used / 4U) * 3U;  // Cannot overflow
    xtr_t* decoded = xtr_new_with_capacity(decoded_len);
    if (decoded == NULL) { return NULL; }
    size_t bin_idx = 0U;
    size_t enc_idx = 0U;
    while (enc_idx < enc->used)
    {
        base64_decode_buffer(&decoded->buffer[bin_idx], &enc->buffer[enc_idx]);
        bin_idx += 3U;
        enc_idx += 4U;
    }
    if (enc->buffer[enc_idx - 1U] == BASE64_PADDING) { decoded->buffer[bin_idx--] = 0U; }
    if (enc->buffer[enc_idx - 2U] == BASE64_PADDING) { decoded->buffer[bin_idx--] = 0U; }
    set_used_and_terminator(decoded, bin_idx);
    return decoded;
}
