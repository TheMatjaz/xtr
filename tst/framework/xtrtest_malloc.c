/**
 * @file
 * Replacements of malloc functions with wrapped versions that fail on
 * command for testing out-of-memory situations.
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

extern void* __real_malloc(size_t len);

extern void* __real_calloc(size_t count, size_t len);

extern void* __real_realloc(void* data, size_t len);

// Declarations of the functions in this file to avoid compiler warning
// about functions that were never declared.
void* __wrap_malloc(size_t len);

void* __wrap_calloc(size_t count, size_t len);

void* __wrap_realloc(void* data, size_t len);

static volatile size_t counter_calls_until_failure = SIZE_MAX;
static volatile bool failing_enabled = false;

void
xtrtest_malloc_fail_after(const size_t count)
{
    counter_calls_until_failure = count;
    failing_enabled = true;
}

void
xtrtest_malloc_disable_failing(void)
{
    failing_enabled = false;
}

void*
__wrap_malloc(size_t len)
{
    if (failing_enabled && counter_calls_until_failure == 0U)
    {
        // Reached point where malloc should fail. Disable failing so other
        // test cases are not affected. It's basically a self-cleanup,
        // so there is no explicit need of calling
        // xtrtest_malloc_disable_failing().
        failing_enabled = false;
        return NULL;
    }
    else
    {
        counter_calls_until_failure--;
        return __real_malloc(len);
    }
}

void*
__wrap_calloc(size_t count, size_t len)
{
    if (failing_enabled && counter_calls_until_failure == 0U)
    {
        // Reached point where calloc should fail. Disable failing so other
        // test cases are not affected. It's basically a self-cleanup,
        // so there is no explicit need of calling
        // xtrtest_malloc_disable_failing().
        failing_enabled = false;
        return NULL;
    }
    else
    {
        counter_calls_until_failure--;
        return __real_calloc(count, len);
    }
}

void*
__wrap_realloc(void* data, size_t len)
{
    if (failing_enabled && counter_calls_until_failure == 0U)
    {
        // Reached point where relloc should fail. Disable failing so other
        // test cases are not affected. It's basically a self-cleanup,
        // so there is no explicit need of calling
        // xtrtest_malloc_disable_failing().
        failing_enabled = false;
        return NULL;
    }
    else
    {
        counter_calls_until_failure--;
        return __real_realloc(data, len);
    }
}
