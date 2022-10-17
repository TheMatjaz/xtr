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

/**
 * @internal
 * Encodes a binary 3-byte buffer into an ASCII 4-byte string.
 *
 *         | AA AA AA aa | BB BB cc cc | CC dd dd dd | = binary/decoded
 *                     \\               //
 *         | AA AA AA | aa BB BB|cc cc CC | dd dd dd | = base64/text
 */
inline static void
base64_encode_buffer(uint8_t* const text, const uint8_t binary[3])
{
    text[0] = BASE64_SYMBOLS[binary[0] >> 2U];
    text[1] = BASE64_SYMBOLS[((binary[0] & 0x03U) << 4U) + (binary[1] >> 4U)];
    text[2] = BASE64_SYMBOLS[((binary[1] & 0x0FU) << 2U) + (binary[2] >> 6U)];
    text[3] = BASE64_SYMBOLS[binary[2] & 0x3FU];
}

/**
 * @internal
 * Decodes a ASCII 4-byte string into a binary 3-byte buffer.
 *
 *         | AA AA AA aa | BB BB cc cc | CC dd dd dd | = binary/binary
 *            :  :  : \\    :  :  :  :  //   :  :  :
 *         | AA AA AA | aa BB BB|cc cc CC | dd dd dd | = base64/text
 */
inline static void
base64_decode_buffer(uint8_t* const binary, const uint8_t text[4])
{
    binary[0] = (uint8_t) ((text[0] << 2U) | ((text[1] >> 4U) & 0x03U));
    binary[1] = (uint8_t) ((text[1] << 4U) | ((text[2] >> 2U) & 0x0FU));
    binary[3] = (uint8_t) ((text[1] << 6U) | (text[2] & 0x3FU));
}

XTR_API xtr_t*
xtr_base64_encode(const xtr_t* const binary)
{
    const size_t b64_text_len = ((binary->used + 2U) * 4U) / 3U;
    if (b64_text_len < (binary->used + 2U)) { return NULL; } // Integer overflow
    xtr_t* const b64_text = xtr_new(b64_text_len);
    if (b64_text == NULL) { return NULL; }
    const size_t remainder = binary->used % 3U;
    const size_t trail_start_idx = binary->used - remainder;
    size_t bin_idx = 0U;
    size_t text_idx = 0U;
    while (bin_idx < trail_start_idx)
    {
        base64_encode_buffer(&b64_text->buffer[text_idx], &binary->buffer[bin_idx]);
        bin_idx += 3U;
        text_idx += 4U;
    }
    uint8_t trail[3U] = {0, 0, 0};
    if (remainder == 2U)
    {
        trail[0] = binary->buffer[trail_start_idx];
        trail[1] = binary->buffer[trail_start_idx + 1U];
        base64_encode_buffer(&b64_text->buffer[text_idx], trail);
        b64_text->buffer[text_idx + 3U] = BASE64_PADDING;
    }
    else if (remainder == 1U)
    {
        trail[0] = binary->buffer[trail_start_idx];
        base64_encode_buffer(&b64_text->buffer[text_idx], trail);
        b64_text->buffer[text_idx + 2U] = BASE64_PADDING;
        b64_text->buffer[text_idx + 3U] = BASE64_PADDING;
    }
    else
    {
        // No remainder, no padding required.
    }
    set_used_and_terminator(b64_text, b64_text_len);
    return b64_text;
    // TODO make padding optional in the encoding
}

// TODO assumes the string does not change while being decoded
XTR_API xtr_t*
xtr_base64_decode(const xtr_t* const b64_text)
{
    if (b64_text == NULL) { return NULL; }
    // TODO shall we support it and truncate it?
    // TODO check all chars are valid
    // TODo support non-padded strings
    // TODO skip whitespace
    // TODO support base64url standard (- instead of +, _ instead of /)
    // TODO support imap mailbox names encoding (, instead of +)
    // TODO make padding optional in the decoding
    // tODO make paddings forbidden parametrically in the encoding
    const size_t binary_len = (b64_text->used / 4U) * 3U;  // Cannot overflow
    xtr_t* binary = xtr_new(binary_len);
    if (binary == NULL) { return NULL; }
    size_t bin_idx = 0U;
    size_t text_idx = 0U;
    size_t buffer_idx = 0U;
    uint8_t buffer[4] = {0};
    while (text_idx < b64_text->used)
    {
        register uint8_t chr = b64_text->buffer[text_idx];
        // Slower, but robust version against invalid chars, whitespace, and
        // different standards of base64 encoding.
        if (chr == '=' && text_idx < -b64_text->used - 2U)
        {
            // TODO string error: declared length is much more than the
            // found length, as a padding character is found too early
            xtr_free(&binary);
            return NULL;
        }
        // TODO make ignoring of whitespace optional
        if (isspace(chr))
        {
            text_idx++;
            continue;
        }
        // TODO make encoding in different formats parameteric
        // base64url encoding
        if (chr == '-') { chr = '+'; }
        else if (chr == '_') { chr = '/'; }
        else if (chr == ',') { chr = '+'; } // IMAP encoding
        if (isalnum(chr) || chr == '+' || chr == '/')
        {
            text_idx++;
            buffer[buffer_idx++] = chr;
            if (buffer_idx == sizeof(buffer))
            {
                base64_decode_buffer(&binary->buffer[bin_idx], buffer);
                bin_idx += 3U;
                buffer_idx = 0U;
            }
        }
        else
        {
            // Invalid base64 character, including null terminator
            xtr_free(&binary);
            return NULL;
        }
    }
    // TODO Remainder section, if there is any trailing data without padding
    // TODO make ignoring of non-padded parametric
    if (b64_text->buffer[text_idx - 1U] == BASE64_PADDING) { binary->buffer[bin_idx--] = 0U; }
    if (b64_text->buffer[text_idx - 2U] == BASE64_PADDING) { binary->buffer[bin_idx--] = 0U; }
    set_used_and_terminator(binary, bin_idx);
    return binary;
}
