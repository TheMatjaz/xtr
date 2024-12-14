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

void
xtrtest_zeros_valid_empty(void)
{
    xtr_t* obtained = xtr_zeros(0);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 0);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_length(obtained), 0);
    atto_neq(xtr_bytes(obtained), NULL);
    atto_zeros(xtr_bytes(obtained), 1);  // NULL terminator
    xtr_free(&obtained);
}

void
xtrtest_zeros_valid_1_byte(void)
{
    xtr_t* obtained = xtr_zeros(1);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 1);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_length(obtained), 1);
    atto_neq(xtr_bytes(obtained), NULL);
    atto_zeros(xtr_bytes(obtained), 1);  // Before NULL terminator
    xtr_free(&obtained);
}

void
xtrtest_zeros_valid_6_bytes(void)
{
    xtr_t* obtained = xtr_zeros(6);
    atto_neq(obtained, NULL);
    atto_eq(xtr_capacity(obtained), 6);
    atto_eq(xtr_available(obtained), 0);
    atto_eq(xtr_length(obtained), 6);
    atto_neq(xtr_bytes(obtained), NULL);
    atto_zeros(xtr_bytes(obtained), 6);  // Before NULL terminator
    xtr_free(&obtained);
}

void
xtrtest_zeros_fail_malloc(void)
{
    xtrtest_malloc_fail_after(0);
    const xtr_t* obtained = xtr_zeros(3);
    atto_eq(obtained, NULL);
}
