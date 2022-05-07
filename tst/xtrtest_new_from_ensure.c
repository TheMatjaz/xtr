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

#include "xtrtest.h"

static void
xtrtest_new_from_ensure_valid_null_string_0_bytes_provides(void)
{
    // Should provide an empty string with 0 available free bytes
    xtr_t* obtained = xtr_new_from_ensure(NULL, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_null_string_6_bytes(void)
{
    // Should provide an empty string with 6 available free bytes
    xtr_t* obtained = xtr_new_from_ensure(NULL, 6);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 6);
    atto_eq(xtr_available(obtained), 6);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_empty_string_0_bytes(void)
{
    // Should provide an empty string with 0 available free bytes
    xtr_t* obtained = xtr_new_from_ensure("", 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_empty_string_1_byte(void)
{
    // Should provide an empty string with 1 available free bytes
    xtr_t* obtained = xtr_new_from_ensure("", 1);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 1);
    atto_eq(xtr_available(obtained), 1);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_empty_string_15_bytes(void)
{
    // Should provide an empty string with 15 available free bytes
    xtr_t* obtained = xtr_new_from_ensure("", 15);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 15);
    atto_eq(xtr_available(obtained), 15);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_1_char_0_bytes(void)
{
    // Should provide a 1-char string with 0 available free bytes
    // because the string is longer than the ensured space
    xtr_t* obtained = xtr_new_from_ensure("a", 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 1);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 1);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "a", 2);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_1_char_1_bytes(void)
{
    // Should provide a 1-char string with 0 available free bytes
    // because the string is equal than the ensured space
    xtr_t* obtained = xtr_new_from_ensure("a", 1);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 1);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 1);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "a", 2);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_6_chars_6_bytes(void)
{
    // Should provide a 6-char string with 0 available free bytes
    // because the string is equal than the ensured space
    xtr_t* obtained = xtr_new_from_ensure("Abcdef", 6);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 6);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 6);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "Abcdef", 7);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_6_chars_4_bytes(void)
{
    // Should provide a 6-char string with 0 available free bytes
    // because the string is longer than the ensured space
    xtr_t* obtained = xtr_new_from_ensure("Abcdef", 4);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 6);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 6);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "Abcdef", 7);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_valid_6_chars_10_bytes(void)
{
    // Should provide a 6-char string with 4 available free bytes
    xtr_t* obtained = xtr_new_from_ensure("Abcdef", 10);
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 10);
    atto_eq(xtr_available(obtained), 4);
    atto_eq(xtr_len(obtained), 6);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "Abcdef", 7);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_ensure_fail_malloc(void)
{
    xtrtest_malloc_fail_after(0);
    xtr_t* obtained = xtr_new_from_ensure("abc", 10);
    atto_eq(obtained, NULL);
}

static void
xtrtest_new_ensure_fail_size_overflow(void)
{
    xtr_t* obtained = xtr_new_from_ensure(NULL, SIZE_MAX);
    atto_eq(obtained, NULL);
    obtained = xtr_new_from_ensure("abc", SIZE_MAX);
    atto_eq(obtained, NULL);
}

void xtrtest_new_from_ensure(void)
{
    xtrtest_new_from_ensure_valid_empty_string_0_bytes();
    xtrtest_new_from_ensure_valid_null_string_0_bytes_provides();
    xtrtest_new_from_ensure_valid_null_string_6_bytes();
    xtrtest_new_from_ensure_valid_empty_string_1_byte();
    xtrtest_new_from_ensure_valid_empty_string_15_bytes();
    xtrtest_new_from_ensure_valid_1_char_0_bytes();
    xtrtest_new_from_ensure_valid_1_char_1_bytes();
    xtrtest_new_from_ensure_valid_6_chars_6_bytes();
    xtrtest_new_from_ensure_valid_6_chars_4_bytes();
    xtrtest_new_from_ensure_valid_6_chars_10_bytes();
    xtrtest_new_from_ensure_fail_malloc();
    xtrtest_new_ensure_fail_size_overflow();
    atto_report();
}
