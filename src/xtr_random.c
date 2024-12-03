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

#include "xtr_internal.h"

// TODO consuder using the OS TRNG call just to initialise a seed and then use
// a local non-crypto PRNG to generate further random strings.

#if XTR_OS == 'w'

    #include <Windows.h> /* To avoid compilation errors for other Windows headers. */
    #include <bcrypt.h>  /* For BCryptGenRandom() - requires explicit linking to `bcrypt` lib. */

XTR_API xtr_t*
xtr_random(const size_t len)
{
    xtr_t* random = xtr_new_with_capacity(len);
    if (random == NULL)
    {
        return NULL;
    }
    size_t generated = 0U;
    while (generated < len)
    {
        // Looping because len (size_t) may be larger than the ULONG (32-bit)
        // buffer size passed to BCryptGenRandom. This would happen if the user
        // requests 4.3+ GB of random data in a single xtring - unlikely,
        // but not impossible. Usually this is a single-iteration loop.
        const NTSTATUS result = BCryptGenRandom(NULL,
                                                &random->buffer[generated],
                                                (ULONG) (len - generated),
                                                BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (result != STATUS_SUCCESS)
        {
            xtr_free(&random);
            return NULL;
        }
        generated += (ULONG) (len - generated);
    }
    return random;
}

#elif XTR_OS == 'u' || XTR_OS == 'm'

    #include <stdio.h> /* For /dev/urandom virtual-file IO */

XTR_API xtr_t*
xtr_random(const size_t len)
{
    xtr_t* random = xtr_new(len);
    if (random == NULL)
    {
        return NULL;
    }
    FILE* rng = fopen("/dev/urandom", "r");
    size_t obtained = len - 1U;  // Different than len to see if it changes
    if (rng != NULL)
    {
        obtained = fread(random->buffer, sizeof(uint8_t), len, rng);
        fclose(rng);
    }
    if (obtained != len)
    {
        xtr_free(&random);
        return NULL;
    }
    return random;
}

#else

    // TODO consider using _Static_assert() and the equivalent for MSVC
    #error "Unsupported OS"

#endif
