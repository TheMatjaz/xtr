/**
 * @file
 * Xtring - Extendible strings for C
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

#ifndef XTR_H
#define XTR_H

/**
 * @def XTR_API
 * Marker of all the library's public API functions. Used to add exporting
 * indicators for DLL on Windows, empty on other platforms.
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__NT__)
/**
 * @def XTR_WINDOWS
 * Indicator simplifying the check for the Windows platform (undefined on other platforms).
 * Used for internal decisions on how to inline functions.
 */
#define XTR_WINDOWS 1
#define XTR_API __declspec(dllexport)
#else
#define XTR_API
#endif

#define XTR_INLINE inline
#define XTR_DESTROYS

/** Major version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_MAJOR 0
/** Minor version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_MINOR 1
/** Bugfix/patch version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_BUGFIX 0
/** Version of this API conforming to semantic versioning as a string. */
#define XTR_API_VERSION "0.1.0"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
// TODO make stdlib optional for embedded

typedef struct xtr xtr_t;

#define XTR_SAFE 1

// New Xtrings
XTR_API xtr_t* xtr_new_ensure(size_t len);
XTR_API xtr_t* xtr_new_empty(void);
XTR_API xtr_t* xtr_new_from(const char* str);
XTR_API xtr_t* xtr_new_from_ensure(const char* str, size_t len);
XTR_API xtr_t* xtr_new_clone(const xtr_t* xtr);
XTR_API xtr_t* xtr_new_clone_ensure(const xtr_t* xtr, size_t len);
XTR_API xtr_t* xtr_new_fill(char c, size_t len);
XTR_API xtr_t* xtr_new_repeat(const char* str, size_t times);

// Free
XTR_API void xtr_free(xtr_t** pxtr);

// Xtring properties
XTR_API size_t xtr_len(const xtr_t* xtr);
XTR_API size_t xtr_allocated(const xtr_t* xtr);
XTR_API size_t xtr_available(const xtr_t* xtr);
XTR_API bool xtr_is_empty(const xtr_t* xtr);
XTR_API const char* xtr_cstring(const xtr_t* xtr);

// Concatenation
XTR_API xtr_t* xtr_merge(const xtr_t* a, const xtr_t* b);
// TODO merge many? Varlena?

// Allocation size
XTR_API xtr_t* xtr_resize(xtr_t* xtr, size_t len);
XTR_API xtr_t* xtr_compress(xtr_t* xtr, size_t len); // TODO No need, just clone

// Comparing
XTR_API int xtr_cmp(const xtr_t* a, const xtr_t* b);
XTR_API size_t xtr_find(const xtr_t* xtr, const xtr_t* sub);
// TODO substring search, like strstr
// TODO case compare
// TODO constant time compare

// TODO iterator?
// TODO utf8 iterator

// TODO is it a good idea to EAT a xtr if it needs reallocation?
// Shouldn't the user free it?
// Maybe consider functions that free internally and some that don't
// like realloc and reallocf
// TODO split between safe and fast functions, don't make it a compile-time
// option, but a runtime option. I may not care for all strings, but for some.
// TODo consider making functions WITHOUT side effects for max clarity, purely funcitonal
XTR_API void xtr_extend(XTR_DESTROYS xtr_t** a, const xtr_t* b);
XTR_API xtr_t* xtr_extend_many(xtr_t* xtr, char character, size_t times);
XTR_API xtr_t* xtr_append(xtr_t* xtr, char character);
XTR_API xtr_t* xtr_append_many(xtr_t* xtr, char character, size_t times);
// TODO padding to size

// Trimming
XTR_API void xtr_clear(xtr_t* xtr);
// TODO rtrim, ltrim whitespace with fast rtrim
// TODO trim allocated space, i.e. realloc smaller

// Utils
//TODO count occurences
//TODO capitalize
//TODO encoding change
//TODO endswith
//TODO startswith
//TODO fmt
//TODO printf
//TODO sprintf
//TODO isascii
//TODO isalnum
//TODO isalpha
//TODO islower
//TODO isdigit
//TODO isnumeric
//TODO isprintable
//TODO isspace
//TODO isupper
//TODO tolower
//TODO toupper
//TODO lstrip with memmove
//TODO partition
//TODO removeprefix - like ltrim but for more chars
//TODO removesuffix - like rtrim but for more chars
//TODO replace
//TODO rfind
//TODO justify
//TODO split
//TODO rsplit
//TODO splitlines
//TODO swapcase
//TODO zerofill
// TODO insert
//TODO prepend
//TODO reverse

#endif /* XTR_H */
