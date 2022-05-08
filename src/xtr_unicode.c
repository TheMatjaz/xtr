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

#define UNICODE_REPLACEMENT_CHAR 0xFFFDU

static void
utf8_encode(uint8_t* encoded, const uint32_t codepoint)
{
    if (codepoint <= 0x7FU)
    {
        // 7 bits, encoded as 0xxx_xxxx
        encoded[0] = (uint8_t) codepoint;
    }
    else if (codepoint <= 0x3FFU)
    {
        // 11 bits, encoded as 110x_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xC0U | ((codepoint >> 6U) & 0x1FU));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0xFFFFU)
    {
        // 16 bits, encoded as 1110_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xE0U | ((codepoint >> 12U) & 0x0FU));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0x1FFFFFU)
    {
        // 21 bits, encoded as 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xF0U | ((codepoint >> 18U) & 0x07U));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[3] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0x3FFFFFFU)
    {
        // 26 bits, encoded as 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xF8U | ((codepoint >> 24U) & 0x03U));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 18U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[3] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[4] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else if (codepoint <= 0x7FFFFFFFU)
    {
        // 31 bits, encoded as 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        encoded[0] = (uint8_t) (0xFCU | ((codepoint >> 30U) & 0x01U));
        encoded[1] = (uint8_t) (0x80U | ((codepoint >> 24U) & 0x3FU));
        encoded[2] = (uint8_t) (0x80U | ((codepoint >> 18U) & 0x3FU));
        encoded[3] = (uint8_t) (0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[4] = (uint8_t) (0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[5] = (uint8_t) (0x80U | ((codepoint >> 0U) & 0x3FU));
    }
    else
    {
        // TODO unsupported codepoint
    }
}

static uint32_t
utf8_decode(const uint8_t* encoded)
{
    while ((*encoded & 0xC0U) == 0x80U)
    {
        // Non-first byte of UTF-8 encoding: 10xx_xxxx
        // Skip until a first byte is found (self-sychronisation).
        encoded++;
    }
    if ((*encoded & 0xC0U) == 0xC0U)
    {
        // First byte of UTF-8 encoding: 11xx_xxxx
        // Search for most significant zero, to decode how many bytes follow.
        register uint_fast8_t bytes_following = 1U;
        register uint8_t mask = 0x20U;
        while (*encoded & mask)
        {
            mask >>= 1U;
            bytes_following++;
        }
        // mask-1 covers all useful bits of the first byte byte
        uint32_t codepoint = encoded[0] & (mask - 1U);
        while (bytes_following--)
        {
            encoded++;
            if ((*encoded & 0xC0U) == 0x80U)
            {
                // Correct non-first byte of UTF-8 encoding: 10xx_xxxx
                codepoint <<= 6U;
                codepoint |= *encoded & 0x3FU;
            }
            else
            {
                // Unexpected byte after first one.
                codepoint = UNICODE_REPLACEMENT_CHAR; // TODO yield
                break;
            }
        }
        return codepoint;
    }
    else
    {
        // First and only byte of UTF-8 encoding: ASCII character, 0xxx_xxxx
        return *encoded;
    }
}
