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
xtrtest_from_str_repeated_with_capacity_valid_null_0_times_0_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity(NULL, 0, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_null_0_times_7_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity(NULL, 0, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 7);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_null_1_time_0_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity(NULL, 1, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_null_1_time_7_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity(NULL, 1, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 7);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_times_0_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("", 0, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_times_7_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("", 0, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 7);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("", 1, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_empty_string_7_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("", 1, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 7);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_0_times_0_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("a", 0, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_0_times_7_space(void)
{
    // Produces an empty xtring
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("a", 0, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 7);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_1_times_0_space(void)
{
    // Produces "a"
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("a", 1, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 1);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 1);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "a", 2);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_1_times_7_space(void)
{
    // Produces "a"
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("a", 1, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 6);
    atto_eq(xtr_len(obtained), 1);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "a", 2);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_2_times_0_space(void)
{
    // Produces "a"
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("a", 2, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 2);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 2);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "aa", 2);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_2_times_7_space(void)
{
    // Produces "a"
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("a", 2, 7);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 7);
    atto_eq(xtr_available(obtained), 5);
    atto_eq(xtr_len(obtained), 2);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "aa", 2);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_6_bytes_string_3_times_0_space(void)
{
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("Abcdef", 3, 0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 6 * 3);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 6 * 3);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "AbcdefAbcdefAbcdef", 6 * 3 + 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_valid_6_bytes_string_3_times_20_space(void)
{
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("Abcdef", 3, 20);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 20);
    atto_eq(xtr_available(obtained), 2);
    atto_eq(xtr_len(obtained), 18);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "AbcdefAbcdefAbcdef", 18 + 1);
    xtr_free(&obtained);
}

static void
xtrtest_from_str_repeated_with_capacity_fail_malloc(void)
{
    xtrtest_malloc_fail_after(0);
    xtr_t* obtained = xtr_from_str_repeated_with_capacity("abc", 2, 0);
    atto_eq(obtained, NULL);
}

void xtrtest_from_str_repeated_with_capacity(void)
{
    xtrtest_from_str_repeated_with_capacity_valid_null_0_times_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_null_0_times_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_null_1_time_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_null_1_time_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_times_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_times_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_empty_string_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_0_times_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_0_times_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_1_times_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_1_times_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_2_times_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_2_times_7_space();
    xtrtest_from_str_repeated_with_capacity_valid_6_bytes_string_3_times_0_space();
    xtrtest_from_str_repeated_with_capacity_valid_6_bytes_string_3_times_20_space();
    xtrtest_from_str_repeated_with_capacity_fail_malloc();
    atto_report();
}
