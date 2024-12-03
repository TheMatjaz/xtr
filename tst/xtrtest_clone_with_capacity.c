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

#include "xtrtest.h"

static void
xtrtest_clone_with_capacity_valid_empty_xtr_same_capacity(void)
{
    xtr_t* original = xtr_new_empty();
    atto_neq(original, NULL);

    xtr_t* clone = xtr_expanded(original, 0);
    atto_neq(clone, NULL);
    atto_neq(original, clone);
    atto_eq(xtr_capacity(original), xtr_capacity(clone));
    atto_eq(xtr_available(original), xtr_available(clone));
    atto_eq(xtr_length(original), xtr_length(clone));
    atto_neq(xtr_cstring(original), xtr_cstring(clone));
    atto_memeq(xtr_cstring(original), xtr_cstring(clone), 1);
    atto_memeq(xtr_cstring(clone), "", 1);

    xtr_free(&original);
    xtr_free(&clone);
}

static void
xtrtest_clone_with_capacity_valid_empty_xtr_more_capacity(void)
{
    xtr_t* original = xtr_new_empty();
    atto_neq(original, NULL);

    xtr_t* clone = xtr_expanded(original, 1);
    atto_neq(clone, NULL);
    atto_neq(original, clone);
    atto_eq(xtr_capacity(clone), 1);
    atto_eq(xtr_available(clone), 1);
    atto_eq(xtr_length(original), xtr_length(clone));
    atto_neq(xtr_cstring(original), xtr_cstring(clone));
    atto_memeq(xtr_cstring(original), xtr_cstring(clone), 1);
    atto_memeq(xtr_cstring(clone), "", 1);

    xtr_free(&original);
    xtr_free(&clone);
}

static void
xtrtest_clone_with_capacity_valid_1_char_xtr_less_capacity(void)
{
    xtr_t* original = xtr_from_str("a");
    atto_neq(original, NULL);

    xtr_t* clone = xtr_expanded(original, 0);
    atto_neq(clone, NULL);
    atto_neq(original, clone);
    atto_eq(xtr_capacity(original), xtr_capacity(clone));
    atto_eq(xtr_available(original), xtr_available(clone));
    atto_eq(xtr_length(original), xtr_length(clone));
    atto_neq(xtr_cstring(original), xtr_cstring(clone));
    atto_memeq(xtr_cstring(original), xtr_cstring(clone), 1);
    atto_memeq(xtr_cstring(clone), "a", 2);

    xtr_free(&original);
    xtr_free(&clone);
}

static void
xtrtest_clone_with_capacity_valid_1_char_xtr_same_capacity(void)
{
    xtr_t* original = xtr_from_str("a");
    atto_neq(original, NULL);

    xtr_t* clone = xtr_expanded(original, 1);
    atto_neq(clone, NULL);
    atto_neq(original, clone);
    atto_eq(xtr_capacity(original), xtr_capacity(clone));
    atto_eq(xtr_available(original), xtr_available(clone));
    atto_eq(xtr_length(original), xtr_length(clone));
    atto_neq(xtr_cstring(original), xtr_cstring(clone));
    atto_memeq(xtr_cstring(original), xtr_cstring(clone), 1);
    atto_memeq(xtr_cstring(clone), "a", 2);

    xtr_free(&original);
    xtr_free(&clone);
}

static void
xtrtest_clone_with_capacity_valid_1_char_xtr_more_capacity(void)
{
    xtr_t* original = xtr_from_str("a");
    atto_neq(original, NULL);

    xtr_t* clone = xtr_expanded(original, 2);
    atto_neq(clone, NULL);
    atto_neq(original, clone);
    atto_eq(xtr_capacity(clone), 2);
    atto_eq(xtr_available(clone), 1);
    atto_eq(xtr_length(original), xtr_length(clone));
    atto_neq(xtr_cstring(original), xtr_cstring(clone));
    atto_memeq(xtr_cstring(original), xtr_cstring(clone), 1);
    atto_memeq(xtr_cstring(clone), "a", 2);

    xtr_free(&original);
    xtr_free(&clone);
}

void
xtrtest_clone_with_capacity(void)
{
    xtrtest_clone_with_capacity_valid_empty_xtr_same_capacity();
    xtrtest_clone_with_capacity_valid_empty_xtr_more_capacity();
    xtrtest_clone_with_capacity_valid_1_char_xtr_less_capacity();
    xtrtest_clone_with_capacity_valid_1_char_xtr_same_capacity();
    xtrtest_clone_with_capacity_valid_1_char_xtr_more_capacity();
    atto_report();
}
