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
xtrtest_is_spaces_valid_null(void)
{
    atto_false(xtr_is_spaces(NULL));
}

static void
xtrtest_is_spaces_valid_empty(void)
{
    xtr_t* obtained = xtr_new();
    atto_false(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}


static void
xtrtest_is_spaces_valid_empty_with_capacity(void)
{
    xtr_t* obtained = xtr_new_with_capacity(1);
    atto_false(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}

static void
xtrtest_is_spaces_valid_non_spaces(void)
{
    xtr_t* obtained = xtr_from_str("abc");
    atto_false(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}

static void
xtrtest_is_spaces_valid_single_space(void)
{
    xtr_t* obtained = xtr_from_str(" ");
    atto_true(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}

static void
xtrtest_is_spaces_valid_many_spaces(void)
{
    xtr_t* obtained = xtr_from_str("    ");
    atto_true(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}

static void
xtrtest_is_spaces_valid_different_whitespaces(void)
{
    xtr_t* obtained = xtr_from_str("    \t  \n  \v \r  ");
    atto_true(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}

static void
xtrtest_is_spaces_valid_not_only_whitespaces(void)
{
    xtr_t* obtained = xtr_from_str("    \t X \n  \v \r  ");
    atto_false(xtr_is_spaces(obtained));
    xtr_free(&obtained);
}

void
xtrtest_is_spaces(void)
{
    xtrtest_is_spaces_valid_null();
    xtrtest_is_spaces_valid_empty();
    xtrtest_is_spaces_valid_empty_with_capacity();
    xtrtest_is_spaces_valid_non_spaces();
    xtrtest_is_spaces_valid_single_space();
    xtrtest_is_spaces_valid_many_spaces();
    xtrtest_is_spaces_valid_different_whitespaces();
    xtrtest_is_spaces_valid_not_only_whitespaces();
    atto_report();
}
