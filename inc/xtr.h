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

#ifdef __cplusplus
extern "C"
{
#endif

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
#define XTR_API_VERSION_MAJOR 0U
/** Minor version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_MINOR 1U
/** Bugfix/patch version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_BUGFIX 0U
/** Version of this API conforming to semantic versioning as a string. */
#define XTR_API_VERSION "0.1.0"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
// TODO make stdlib optional for embedded

/** Opaque xtring structure. */
typedef struct xtr xtr_t;

#define XTR_CLEAR_HEAP 1

// =================== NEW XTRINGS ============================================
// ------------------- New empty xtrings ------------------------------------------
/**
 * Constructs a new empty xtring with no pre-allocated free space.
 * Equivalent to xtr_new_with_capacity(0).
 * @return the new xtring or NULL in case of malloc failure or integer overflows.
 */
XTR_API xtr_t* xtr_new(void);

/**
 * Constructs a new empty xtring with `max_len` bytes pre-allocated free space.
 * @param [in] max_len length the xtring could be expanded to without reallocating.
 * @return the new xtring or NULL in case of malloc failure or integer overflows.
 */
XTR_API xtr_t* xtr_new_with_capacity(size_t max_len);


XTR_API void xtr_free(xtr_t** pxtr);

// ------------------- New initialised xtrings from other data ------------------------------------
XTR_API xtr_t* xtr_zeros(size_t len);

XTR_API xtr_t*
xtr_from_str(const char* str);

XTR_API xtr_t*
xtr_from_str_with_capacity(const char* str, size_t at_least);

XTR_API xtr_t*
xtr_from_str_repeated(const char* str, size_t amount);

XTR_API xtr_t*
xtr_from_str_repeated_with_capacity(const char* str,
                                    size_t amount,
                                    size_t at_least);

XTR_API xtr_t*
xtr_from_array(const uint8_t* array,
               size_t array_len);

XTR_API xtr_t*
xtr_from_array_with_capacity(const uint8_t* array,
                             size_t array_len,
                             size_t at_least);

XTR_API xtr_t*
xtr_from_array_repeated(const uint8_t* array,
                        size_t array_len,
                        size_t repetitions);

XTR_API xtr_t*
xtr_from_array_repeated_with_capacity(const uint8_t* array,
                                      size_t array_len,
                                      size_t repetitions,
                                      size_t at_least);

XTR_API xtr_t*
xtr_from_byte(uint8_t byte);

XTR_API xtr_t*
xtr_from_byte_repeated(uint8_t byte, size_t len);

XTR_API xtr_t*
xtr_from_byte_repeated_with_capacity(uint8_t byte, size_t len, size_t at_least);

XTR_API xtr_t*
xtr_random(size_t len); // TODO

/**
 * Constructs a new xtring, copying the content of a C-string, while ensuring
 * \p at_least bytes are being allocated overall.
 *
 * The amount of pre-allocated free space at the end of the data will be
 * `max{0, at_least - strlen(str)}`.
 *
 * @param [in] str data to copy into the xtring.
 * @param [in] at_least minimum size of the internal buffer, used to ensure
 * a maximum xtring size without reallocation. If `at_least < strlen(str)`,
 * then `strlen(str)` will be allocated instead.
 * @return the new xtring or NULL in case of malloc failure or integer overflows or NULL input.
 */

// ------------------- New cloned xtrings ------------------------------------
XTR_API xtr_t* xtr_clone(const xtr_t* xtr);

XTR_API xtr_t* xtr_clone_with_capacity(const xtr_t* xtr, size_t max_len);


// ------------------- Xtring properties (getters) ------------------------------------
XTR_API size_t xtr_len(const xtr_t* xtr);

XTR_API size_t xtr_capacity(const xtr_t* xtr);

XTR_API size_t xtr_available(const xtr_t* xtr);

XTR_API const char* xtr_cstring(const xtr_t* xtr);

XTR_API const char* xtr_last(const xtr_t* xtr);

// ------------------- Single-Xtrings content analysis ------------------------------------

XTR_API bool xtr_is_empty(const xtr_t* xtr);

XTR_API bool xtr_is_spaces(const xtr_t* xtr);

XTR_API bool xtr_is_zeros(const xtr_t* xtr);

XTR_API bool xtr_is_zeros_consttime(const xtr_t* xtr);

XTR_API bool xtr_is_not_zeros(const xtr_t* a);

XTR_API bool xtr_is_not_zeros_consttime(const xtr_t* a);

// ------------------- Xtring equality check ------------------------------------

XTR_API int
xtr_cmp_length(const xtr_t* a, const xtr_t* b);

XTR_API int
xtr_cmp_content(const xtr_t* a, const xtr_t* b);

XTR_API bool
xtr_is_equal_length(const xtr_t* a, const xtr_t* b);

XTR_API bool
xtr_is_equal(const xtr_t* a, const xtr_t* b);

XTR_API bool
xtr_is_equal_consttime(const xtr_t* a, const xtr_t* b);

// ------------------- Shorten the Xtring's content ------------------------------------
XTR_API void
xtr_clear(xtr_t* xtr);

XTR_API xtr_t* xtr_pop(xtr_t* xtr, size_t len);

XTR_API void
xtr_rtrim(xtr_t* xtr, const char* chars);

XTR_API void // O(n) operation!
xtr_ltrim(xtr_t* xtr, const char* chars);

// ------------------- Alter the Xtring's allocated memory ------------------------------------

XTR_API xtr_t* xtr_resize(xtr_t* xtr, size_t len);

XTR_API xtr_t* xtr_resize_double(xtr_t* xtr);

XTR_API xtr_t* xtr_resize_free(xtr_t** pxtr, size_t len);

XTR_API xtr_t* xtr_resize_free_double(xtr_t** pxtr);

XTR_API xtr_t* xtr_compress_free(xtr_t** pxtr);


// ------ OTHER
XTR_API xtr_t* xtr_reversed(const xtr_t* xtr);

XTR_API void xtr_reverse(xtr_t* xtr);

XTR_API size_t xtr_occurrences(const xtr_t* xtr, const xtr_t* pattern);

XTR_API xtr_t**
xtr_split_at(const xtr_t* xtr, const xtr_t* pattern);

XTR_API xtr_t**
xtr_split_every(const xtr_t* xtr, size_t part_len);

XTR_API xtr_t**
xtr_split_into(const xtr_t* xtr, size_t parts_amount);



// Concatenation
XTR_API xtr_t* xtr_merge(const xtr_t* a, const xtr_t* b);

XTR_API xtr_t* xtr_repeat(const xtr_t* xtr, size_t repetitions);
// TODO merge many? Varlena?



// Comparing
XTR_API int xtr_cmp_c(const xtr_t* a, const char* b);


XTR_API bool xtr_equal_until(const xtr_t* a, const xtr_t* b, size_t len);

XTR_API bool xtr_equal_shortest(const xtr_t* a, const xtr_t* b);

XTR_API bool xtr_equal_c(const xtr_t* a, const char* b);

XTR_API bool xtr_not_equal(const xtr_t* a, const xtr_t* b);

XTR_API bool xtr_not_equal_c(const xtr_t* a, const char* b);


XTR_API bool xtr_startswith(const xtr_t* a);

XTR_API bool xtr_endswith(const xtr_t* a);

XTR_API bool xtr_contains(const xtr_t* xtr, const xtr_t* pattern);

XTR_API const char* xtr_find(const xtr_t* xtr, const xtr_t* pattern);

XTR_API const char* xtr_find_from(const xtr_t* xtr, const xtr_t* pattern,
                                  size_t start);

XTR_API const char* xtr_find_in(const xtr_t* xtr, const xtr_t* pattern,
                                size_t start, size_t end);
// TODO substring search, like strstr
// TODO case compare
// TODO constant time compare

// TODO iterator?
// TODO utf8 iterator

// TODO split between safe and fast functions, don't make it a compile-time
// option, but a runtime option. I may not care for all strings, but for some.
// TODo consider making functions WITHOUT side effects for max clarity, purely funcitonal
XTR_API void xtr_extend(xtr_t** pbase, const xtr_t* ext);

XTR_API void xtr_extend_many(xtr_t** pbase, const xtr_t* ext, size_t repetitions);

XTR_API void xtr_extend_from(xtr_t** pbase, const char* ext);

XTR_API void xtr_extend_from_many(xtr_t** pbase, const char* ext, size_t repetitions);

XTR_API void xtr_append(xtr_t** pbase, char character);

XTR_API void xtr_append_many(xtr_t** pbase, char character, size_t repetitions);

XTR_API void xtr_prepend(xtr_t** pbase, char character);

XTR_API void xtr_prepend_many(xtr_t** pbase, char character, size_t repetitions);
// TODO prepending another char* or xtr
// TODO padding to size

// Trimming
XTR_API void xtr_clear(xtr_t* xtr);

XTR_API void xtr_ltrim(xtr_t* xtr, const char* chars);

XTR_API void xtr_rtrim(xtr_t* xtr, const char* chars);

XTR_API void xtr_remove_suffix(xtr_t* xtr, const char* str);

XTR_API void xtr_remove_prefix(xtr_t* xtr, const char* str);

// Printing
XTR_API xtr_t* xtr_hex(const xtr_t* xtr);

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
//TODO partition
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
// TODO base64
// TODO hex
// TODO tokenise
// TODO parse as u16, u32, u64, f16, f32, f64, LE and BE - or maybe just iterate?
// TODO different comparisons if one string is shorter than the others
//// TODO xtr_cmp_content should provide also the lexicographical difference between the strings,
//// i.e. the byte1-byte2 difference as signed output
// TODO file extension
// TODO filepath operations WIN + UNIX

#ifdef __cplusplus
}
#endif

#endif  /* XTR_H */
