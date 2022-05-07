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
xtrtest_new_from_valid_empty_string(void)
{
    xtr_t* obtained = xtr_new_from("");
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 0);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "", 1);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_valid_1_byte_string(void)
{
    xtr_t* obtained = xtr_new_from("a");
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 1);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 1);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "a", 2);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_valid_6_bytes_string(void)
{
    xtr_t* obtained = xtr_new_from("Abcdef");
    atto_neq(obtained, NULL);
    atto_eq(xtr_maxlen(obtained), 6);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_len(obtained), 6);
    atto_neq(xtr_cstring(obtained), NULL);
    atto_memeq(xtr_cstring(obtained), "Abcdef", 7);
    xtr_free(&obtained);
}

static void
xtrtest_new_from_fail_malloc(void)
{
    xtrtest_malloc_fail_after(0);
    xtr_t* obtained = xtr_new_from("abc");
    atto_eq(obtained, NULL);
}

static void
xtrtest_new_from_fail_null(void)
{
    xtr_t* obtained = xtr_new_from(NULL);
    atto_eq(obtained, NULL);
}

void xtrtest_new_from(void)
{
    xtrtest_new_from_valid_empty_string();
    xtrtest_new_from_valid_1_byte_string();
    xtrtest_new_from_valid_6_bytes_string();
    xtrtest_new_from_fail_malloc();
    xtrtest_new_from_fail_null();
    atto_report();
}
