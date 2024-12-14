/**
 * @file
 * Xtring - Extendable strings for C
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

#ifndef XTR_H
#define XTR_H

#ifdef __cplusplus
extern "C"
{
#endif

// =================== Macros ============================================
// ------------------- Version ---------------------------------------

/** Major version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_MAJOR 0U
/** Minor version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_MINOR 1U
/** Bugfix/patch version of this API conforming to semantic versioning. */
#define XTR_API_VERSION_BUGFIX 0U
/** Version of this API conforming to semantic versioning as a string. */
#define XTR_API_VERSION "0.1.0"

// ------------------- OS selection --------------------------------------
/**
 * @def XTR_OS
 * Marker of the detected operating system the library is compiled for.
 * A single-character value:
 * - 'W' = 87 = 0x57 for Microsoft Windows
 * - 'L' = 76 = 0x4C for GNU/Linux
 * - 'U' = 85 = 0x55 for Unix
 * - 'M' = 77 = 0x4D for Apple macOS
 * - '\0' = 0 = 0x00 for other, unknown, unsupported
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__NT__)
    #define XTR_OS 'W'
#elif defined(__APPLE__) || defined(__MACH__)
    #define XTR_OS 'M'
#elif defined(__linux__) || defined(linux) || defined(__linux)
    #define XTR_OS 'L'
#elif defined(unix) || defined(__unix) || defined(__unix__)
    #define XTR_OS 'U'
#else
    #define XTR_OS 0
#endif

/**
 * @def XTR_API
 * Marker of all the library's public API functions. Used to add exporting
 * indicators for DLL on Windows, empty on other platforms.
 */
#if XTR_OS == 'W'
    #define XTR_API __declspec(dllexport)
#else
    #define XTR_API
#endif

#ifndef XTR_INLINE
    #define XTR_INLINE inline
#endif

// ------------------- Includes --------------------------------------

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(XTR_INPUT_ASSERTS) && XTR_INPUT_ASSERTS
    #include <assert.h>
    #define XTR_ASSERT(x) assert(x)
#else
    #define XTR_ASSERT(x)
#endif
#if defined(XTR_STDIO) && XTR_STDIO
    #include <stdio.h>
#endif
// ------------------- Dynamic memory management ---------------------

/**
 * @def XTR_MALLOC
 * @def XTR_CALLOC
 * @def XTR_REALLOC
 * @def XTR_FREE
 * Overridable aliases for any custom dynamic memory allocator.
 */
#ifndef XTR_MALLOC
    #define XTR_MALLOC malloc
#endif
#ifndef XTR_CALLOC
    #define XTR_CALLOC calloc
#endif
#ifndef XTR_REALLOC
    #define XTR_REALLOC realloc
#endif
#ifndef XTR_FREE
    #define XTR_FREE free
#endif

/**
 * @def XTR_CLEAR_HEAP
 * Clears (zero-out) the heap memory on allocation and before freeing it.
 *
 * Recommended for higher memory safety assurance, at the cost of additional
 * O(n) operation on construction and destruction.
 *
 * Basically: `calloc()` instead of `malloc()` and `memset(...,0,...)` just
 * before `free()`.
 */
#ifndef XTR_CLEAR_ALLOCATED
    #define XTR_CLEAR_ALLOCATED 1
#endif

// ------------------- Constants --------------------------------------
// Assuming enough memory
#define XTR_MAX_CAPACITY   (SIZE_MAX - sizeof(size_t) * 2U - 1U)

#define XTR_UNKNOWN_STRLEN SIZE_MAX

/** Search failure value, larger than any possible index. */
#define XTR_NOT_FOUND SIZE_MAX

/** Alias for freeing previous xtring after reallocation. */
#define XTR_FREE_OLD true

/** Alias for not freeing previous xtring after reallocation. */
#define XTR_KEEP_OLD false

// =================== Data types ============================================

/**
 * Opaque xtring structure.
 *
 * Keeps track of the data length, of the allocated buffer length (which
 * may be longer than the data), keeps everything null-terminated for
 * compatibility with the C-strings.
 *
 *                                +-- content always null-terminated
 *                                |
 *                                |          +-- buffer always null-terminated
 *                                |          |
 *                                v          v
 *
 *         [capacity][used][abcde\0.........\0]
 *
 *                          \___/            used (5, excl. null terminator)
 *                               \_________/ available free space (11)
 *                          \______________/ capacity (16, excl. null term.)
 */
typedef struct xtr xtr_t;

// =================== NEW XTRINGS ============================================
// ------------------- New empty xtrings ------------------------------------------
/**
 * Allocates an empty xtring with no additional pre-allocated free space.
 *
 * Equivalent to `xtr_new(0)`.
 *
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_new_empty(void);

/**
 * Allocates empty xtring with `capacity` bytes of pre-allocated free space.
 *
 * The pre-allocated capacity allows to expand the string in-place without
 * the need to reallocated until the `capacity` limit is exceeded.
 *
 *             capacity (buffer size)
 *             __________________
 *            /                  \
 *           [abcde...............] buffer
 *            \___/\_____________/
 *           length       available
 *         (used space)  (free space)
 *
 * @param [in] capacity maximum length the xtring could be expanded to without reallocating.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_new(size_t capacity);

/**
 * Frees the allocated memory of xtring after usage and sets the xtring pointer
 * to NULL to avoid use-after-free.
 *
 * Example:
 *         xtr_t* xtring = xtr_new_empty();
 *         // ... usage of the xtring
 *         xtr_free(&xtring);
 *         assert(xtring == NULL);
 *
 * @param [in,out] pxtr **address** of the xtring-pointer.
 */
XTR_API void
xtr_free(xtr_t** pxtr);

// ------------------- New initialised xtrings from other data ------------------------------------
/**
 * New xtring filled with zero-values bytes, similar to `calloc`.
 *
 * @param [in] len length in bytes = amount of zeros. `0` for empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_zeros(size_t len);

/**
 * New xtring initialised with the `len` cryptographically-secure random bytes.
 *
 * On Windows it obtains the random bytes using `BCryptGenRandom`, on
 * Unix-like systems it uses `/dev/urandom`.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] len amount of random bytes.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_random(size_t len);

/**
 * New xtring initialised with the C-string content.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str(const char* str);

#define c2x xtr_from_str

/**
 * New xtring initialised with the C-string content and overall `at_least` allocated
 * space.
 *
 * Ensures the `at_least - strlen(str)` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param [in] str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `strlen(str)` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str_capac(const char* str, size_t at_least);

/**
 * New xtring initialised with the C-string content repeated `repetitions` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @param [in] repetitions amount of times to repeat `str`. `0` for an empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str_repeat(const char* str, size_t repetitions);

/**
 * New xtring initialised with the C-string content repeated `repetitions` times
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - strlen(str)` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param [in] str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @param [in] repetitions amount of times to repeat `str`. `0` for an empty xtring.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `strlen(str) * repetitions` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str_repeat_capac(const char* str, size_t repetitions, size_t at_least);

/**
 * New xtring initialised with the array content.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param [in] array_len amount of bytes to copy from the array.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_bytes(const uint8_t* array, size_t array_len);

/**
 * New xtring initialised with the array content and overall `at_least` allocated
 * space.
 *
 * Ensures the `at_least - array_len` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param [in] array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param [in] array_len amount of bytes to copy from the array.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `array_len` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_bytes_capac(const uint8_t* array, size_t array_len, size_t at_least);

/**
 * New xtring initialised with the array content repeated `repetitions` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param [in] array_len amount of bytes to copy from the array.
 * @param [in] repetitions amount of times to repeat `array`.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_bytes_repeat(const uint8_t* array, size_t array_len, size_t repetitions);

/**
 * New xtring initialised with the C-string content repeated `repetitions` times
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - array_len` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param [in] array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param [in] array_len amount of bytes to copy from the array.
 * @param [in] repetitions amount of times to repeat `array`.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `array_len * repetitions` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_bytes_repeat_capac(const uint8_t* array,
                            size_t array_len,
                            size_t repetitions,
                            size_t at_least);

/**
 * New xtring of length 1 initialised with a single byte.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] byte 8-bit value to place in the xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_byte(uint8_t byte);

/**
 * New xtring initialised with the `byte` repeated `repetitions` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] byte 8-bit value to place in the xtring.
 * @param [in] repetitions amount of times to repeat `byte`.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_byte_repeat(uint8_t byte, size_t repetitions);

/**
 * New xtring initialised with the `byte` repeated `repetitions` times
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - repetitions` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] byte 8-bit value to place in the xtring.
 * @param [in] repetitions amount of times to repeat `byte`.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `repetitions` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_byte_repeat_capac(uint8_t byte, size_t repetitions, size_t at_least);

// ------------------- New cloned xtrings ------------------------------------
/**
 * New xtring with same content and buffer size as another one (copy by value).
 *
 * @param [in] xtr to copy.
 * @return the new xtring or NULL in case of malloc failure or when `xtr` is NULL.
 */
XTR_API xtr_t*
xtr_clone(const xtr_t* xtr);

/**
 * New xtring with same content as another one (copy by value)
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - xtr_length(xtr)` available allocated free space at the
 * clone's end, to have some space ready for expansions without reallocation.
 * @param [in] xtr to copy. Returns NULL if `xtr` is NULL.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `xtr_length(xtr)` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure or when `xtr` is NULL.
 */
XTR_API xtr_t*
xtr_expanded(const xtr_t* xtr, size_t at_least);

/**
 * Reallocates the xtring into a larger buffer, optionally freeing the previous one.
 *
 * Ensures the `at_least - xtr_length(xtr)` available allocated free space at the
 * clone's end, to have some space ready for expansions without reallocation.
 * @param [in,out] pxtr point to the original xtring to copy. Pointed xtr_t*
 *        will be replaced by a larger copy, if a reallocation happens.
 *        Frees the previous xtring. Untouched on failure.
 * @param [in] at_least minimum amount of bytes to allocate, but
 *        `xtr_length(*pxtr)` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure or when `pxtr` or `*pxtr` is NULL.
 *         Note: on success the returned value matches `*pxtr`. On failure the
 *         returned value is NULL and *pxtr is unchanged.
 */
XTR_API xtr_t*
xtr_expand(xtr_t** pxtr, size_t at_least);

/**
 * New xtring with same content as another one (copy by value)
 * but truncated to `max_len` bytes.
 *
 * The first `max_len` bytes are copied over to the new xtring.
 * Does not reserve any additional capacity for expansions. For that use
 * xtr_truncate_tail() or xtr_expanded().
 * @param [in] xtr to reduce. Returns NULL if `xtr` is NULL.
 * @param [in] at_most maximum amount of bytes to copy, automatically
 *        limited to at most `xtr_length(xtr)`.
 * @return the new xtring or NULL in case of malloc failure or when `xtr` is NULL.
 */
XTR_API xtr_t*
xtr_truncated(const xtr_t* xtr, size_t at_most);

/**
 * Reallocates the xtring into a smaller buffer, freeing the previous one,
 * truncating the content to `max_len` bytes.
 *
 * The first `max_len` bytes are copied over to the new xtring.
 * Does not reserve any additional capacity for expansions. For that use
 * xtr_truncate_tail() or xtr_expand().
 * @param [in,out] pxtr point to the original xtring to copy. Pointed xtr_t*
 *        will be replaced by a smaller copy, if a reallocation happens.
 *        Frees the previous xtring. Untouched on failure.
 * @param [in] at_most maximum amount of bytes to copy, automatically
 *        limited to at most `xtr_length(xtr)`.
 * @return the new xtring or NULL in case of malloc failure or when `pxtr` or `*pxtr` is NULL.
 *         Note: on success the returned value matches `*pxtr`. On failure the
 *         returned value is NULL and *pxtr is unchanged.
 */
XTR_API xtr_t*
xtr_truncate(xtr_t** pxtr, size_t at_most);

// ------------------- Xtring properties (getters) ------------------------------------
/**
 * Xtring length = amount of currently **used** bytes in the allocated buffer.
 *
 *             capacity (buffer size)
 *             __________________
 *            /                  \
 *           [abcde...............] buffer
 *            \___/\_____________/
 *           length       available
 *         (used space)  (free space)
 *
 * @param [in] xtr xtring to inspect.
 * @return the length or 0 if `xtr` is NULL.
 */
XTR_API size_t
xtr_length(const xtr_t* xtr);

#define xtr_len xtr_length

/**
 * Buffer size of the xtring = **maximum** possible length the xtring can reach without
 * reallocating.
 *
 *             capacity (buffer size)
 *             __________________
 *            /                  \
 *           [abcde...............] buffer
 *            \___/\_____________/
 *           length       available
 *         (used space)  (free space)
 *
 * @param [in] xtr xtring to inspect.
 * @return the capacity or 0 if `xtr` is NULL.
 */
XTR_API size_t
xtr_capacity(const xtr_t* xtr);

#define xtr_cap xtr_capacity

/**
 * Available xtring space =  amount of **free** bytes that can be appended to the xtring
 * before reallocating.
 *
 *             capacity (buffer size)
 *             __________________
 *            /                  \
 *           [abcde...............] buffer
 *            \___/\_____________/
 *           length       available
 *         (used space)  (free space)
 *
 * @param [in] xtr xtring to inspect.
 * @return the capacity or 0 if `xtr` is NULL.
 */
XTR_API size_t
xtr_available(const xtr_t* xtr);

#define xtr_avail xtr_available

/**
 * Read-only access to the internal buffer as C-string, null-terminated.
 *
 * DO NOT USE IT TO WRITE DATA. It will corrupt the internal structure.
 *
 * @param [in] xtr xtring to inspect.
 * @return null-terminated char array or NULL if `xtr` is NULL.
 */
XTR_API const char*
xtr_cstring(const xtr_t* xtr);

#define xtr_cstr xtr_cstring
#define x2c      xtr_cstring

/**
 * Read-only access to the internal buffer as `uint8_t[]`, null-terminated.
 *
 * DO NOT USE IT TO WRITE DATA. It will corrupt the internal structure.
 *
 * Although the null-termination is not required for binary arrays, it's
 * included due to the internal construction.
 * @param [in] xtr xtring to inspect.
 * @return null-terminated `uint8_t` array or NULL if `xtr` is NULL.
 */
XTR_API const uint8_t*
xtr_bytes(const xtr_t* xtr);

/**
 * Read-only pointer to the last byte of the string.
 *
 * @param [in] xtr xtring to inspect.
 * @return pointer or NULL if `xtr` is NULL or empty.
 */
XTR_API const uint8_t*
xtr_last(const xtr_t* xtr);

// ------------------- Single-Xtrings content analysis ------------------------------------

/**
 * Checks whether the xtring contains any bytes.
 * @param [in] xtr xtring to inspect.
 * @return true if `xtr` is empty (length == 0) or NULL, false otherwise.
 */
XTR_API bool
xtr_is_empty(const xtr_t* xtr);

/**
 * Checks whether the xtring contains only whitespace characters.
 * @param [in] xtr xtring to inspect.
 * @return true if `xtr` contains only `isspace()` characters, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_spaces(const xtr_t* xtr);

/**
 * Checks whether the xtring contains only zero-valued bytes.
 * @param [in] xtr xtring to inspect.
 * @return true if `xtr` contains only zeros, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_zeros(const xtr_t* xtr);

/**
 * Like xtr_is_zeros() but with constant runtime for security application.
 *
 * The entire string is always scanned, even if a non-zero byte is found early.
 * @param [in] xtr xtring to inspect.
 * @return true if `xtr` contains only zeros, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_zeros_consttime(const xtr_t* xtr);

/**
 * Checks whether the xtring is not full of just zero-valued bytes.
 * @param [in] xtr xtring to inspect.
 * @return true if `xtr` contains any non-zero byte, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_not_zeros(const xtr_t* xtr);

/**
 * Like xtr_is_not_zeros() but with constant runtime for security application.
 *
 * The entire string is always scanned, even if xtr non-zero byte is found early.
 * @param [in] xtr xtring to inspect.
 * @return true if `xtr` contains any non-zero byte, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_not_zeros_consttime(const xtr_t* xtr);

// ------------------- Xtring equality check and comparison ------------------------------------

/**
 * Compares just the lengths of the xtrings (amount of used bytes).
 *
 * Mimics `memcmp()` but just for the xtring lengths, not their content.
 * For that, use xtr_cmp().
 * @param [in] a first xtring
 * @param [in] b second xtring
 * @return
 * - `0` when they have equal length: both NULL, or both same length, or both same pointer
 * - negative when `a` is shorter
 *   - `-1` when `a` is NULL and `b` is not (`a` has no data, thus shorter than `b`)
 *   - `-2` when `a` is shorter than `b`
 * - positive when `a` is longer
 *   - `+1` when `b` is NULL and `a` is not (`b` has no data, thus shorter than `a`)
 *   - `+2` when `b` is shorter than `a`
 */
XTR_API int
xtr_cmp_length(const xtr_t* a, const xtr_t* b);

/**
 * Wrapper around xtr_cmp_length() checking only for length equality (amount of used bytes).
 *
 * @param [in] a first xtring
 * @param [in] b second xtring
 * @return true when both are NULL or have equal length
 */
XTR_API bool
xtr_is_equal_length(const xtr_t* a, const xtr_t* b);

/**
 * Compares the two xtrings in length and content, useful for sorting.
 *
 * Mimics `memcmp()`, but compares also the length and NULL pointers.
 * @param [in] a first xtring
 * @param [in] b second xtring
 * @return
 * - `0` when they are equal: both NULL, or both same length and content, or both same pointer
 * - negative when `a` is shorter
 *   - `-1` when `a` is NULL and `b` is not (`a` has no data, thus shorter than `b`)
 *   - `-2` when `a` entirely matches the initial part of `b` but `b` is longer
 *   - `-3` when `a` is contains a byte which has a lower value than in `b`
 * - positive when `a` is longer
 *   - `+1` when `b` is NULL and `a` is not (`b` has no data, thus shorter than `a`)
 *   - `+2` when `b` entirely matches the initial part of `a` but `a` is longer
 *   - `+3` when `b` is contains a byte which has a lower value than in `a`
 */
XTR_API int
xtr_cmp(const xtr_t* a, const xtr_t* b);

/**
 * Checks if two xtrings have exactly the same length and content.
 *
 * @param [in] a first xtring
 * @param [in] b second xtring
 * @return true when both NULL, or both are the same pointer, or both have the same length
 * and same content (used bytes). False otherwise.
 */
XTR_API bool
xtr_is_equal(const xtr_t* a, const xtr_t* b);

/**
 * Like xtr_is_equal() but with constant runtime for security application.
 *
 * Both xtrings are fully scanned, even if a differing byte is found early.
 * EXCEPTION: if one of them is shorter, the function returns IMMEDIATELY,
 * because they cannot be the same string.
 *
 * @param [in] a first xtring
 * @param [in] b second xtring
 * @return true when both NULL, or both are the same pointer, or both have the same length
 * and same content (used bytes). False otherwise.
 */
XTR_API bool
xtr_is_equal_consttime(const xtr_t* a, const xtr_t* b);

/**
 * True if the xtring's head matches the provided prefix.
 *
 * Example:
 *         xtr_startswith(xtr["abcdef"], xtr["abc"]) --> true
 *         xtr_startswith(xtr["abcdef"], xtr["abcdef"]) --> true
 *         xtr_startswith(NULL, NULL) --> true
 *         xtr_startswith(xtr["abcdef"], xtr["xyz"]) --> false
 *         xtr_startswith(xtr["abcdef"], NULL= --> false
 *         xtr_startswith(NULL, xtr["abc"]) --> false
 *         xtr_startswith(xtr["abcdef"], xtr["abcdefghijklm"]) --> false
 *
 * @param [in] xtr to check the beginning of
 * @param [in] prefix to check whether matches `xtr`'s head
 * @return true if `xtr` starts with `prefix` or if they are both NULL, false otherwise.
 *         NOTE: false includes the case of only one being NULL or if the
 *         provided `prefix` is shorter than `xtr`
 */
XTR_API bool
xtr_startswith(const xtr_t* xtr, const xtr_t* prefix);

/**
 * True if the xtring's tail matches the provided suffix.
 *
 * Example:
 *         xtr_endswith(xtr["abcdef"], xtr["def"]) --> true
 *         xtr_endswith(xtr["abcdef"], xtr["abcdef"]) --> true
 *         xtr_endswith(NULL, NULL) --> true
 *         xtr_endswith(xtr["abcdef"], xtr["xyz"]) --> false
 *         xtr_endswith(xtr["abcdef"], xtr["fed"]) --> false
 *         xtr_endswith(xtr["abcdef"], NULL= --> false
 *         xtr_endswith(NULL, xtr["abc"]) --> false
 *         xtr_endswith(xtr["abcdef"], xtr["abcdefghijklm"]) --> false
 *
 * @param [in] xtr to check the end of
 * @param [in] suffix to check whether matches `xtr`'s tail
 * @return true if `xtr` ends with `suffix` or if they are both NULL, false otherwise.
 *         NOTE: false includes the case of only one being NULL or if the
 *         provided `suffix` is shorter than `xtr`
 */
XTR_API bool
xtr_endswith(const xtr_t* xtr, const xtr_t* suffix);

XTR_API bool
xtr_is_spaces(const xtr_t* xtr);
XTR_API bool
xtr_is_alpha(const xtr_t* xtr);
XTR_API bool
xtr_is_alphanum(const xtr_t* xtr);
XTR_API bool
xtr_is_digits(const xtr_t* xtr);
XTR_API bool
xtr_is_upper(const xtr_t* xtr);
XTR_API bool
xtr_is_lower(const xtr_t* xtr);
XTR_API bool
xtr_is_printable(const xtr_t* xtr);

// ------------------- Shorten the Xtring's content ------------------------------------
/**
 * Empties the string, cutting its length to zero, without reallocation.
 *
 * The allocated memory (capability) remains the same. To shorten the buffer
 * use xtr_compress_free();
 * @param [in,out] xtr xtring to erase.
 */
XTR_API void
xtr_clear(xtr_t* xtr);

/**
 * Removes `len` bytes from the xtring start and returns them as a new xtring.
 *
 * Example of popping 3 bytes:
 *
 *         xtr before: "Hello world!"
 *         xtr after:  "lo world!"
 *         returned:   "Hel"
 *
 * O(n) operation, as it requires the rest of the altered string to be
 * shifted with `memmove`.
 * @param [in,out] xtr xtring to pop from an alter, modified in-place.
 * @param [in] len amount of bytes to pop. If more than the length of `xtr`, a copy
 *        of the entire `xtr` is provided and the modified `xtr` is emptied.
 * @return new xtring with the popped prefix of `xtr`, with the bytes in the
 *        same order as they appeared in the original.
 */
XTR_API xtr_t*
xtr_pop_head(xtr_t* xtr, size_t len);

/**
 * Removes `len` bytes from the xtring end and returns them as a new xtring.
 *
 * Example of popping 3 bytes:
 *         xtr before: "Hello world!"
 *         xtr after:  "Hello wor"
 *         returned:   "ld!"
 *
 * O(1) operation.
 * @param [in,out] xtr xtring to pop from an alter, modified in-place.
 * @param [in] len amount of bytes to pop. If more than the length of `xtr`, a copy
 *        of the entire `xtr` is provided and the modified `xtr` is emptied.
 * @return new xtring with the popped prefix of `xtr`, with the bytes in the
 *        same order as they appeared in the original.
 */
XTR_API xtr_t*
xtr_pop_tail(xtr_t* xtr, size_t len);

/**
 * Removes `len` bytes from the xtring start.
 *
 * Like xtr_pop_head() without returning the popped value.
 *
 * Example of truncating 3 bytes:
 *         xtr before: "Hello world!"
 *         xtr after:  "lo world!"
 *
 * O(n) operation, as it requires the rest of the altered string to be
 * shifted with `memmove`.
 * @param [in,out] xtr xtring to truncate, modified in-place.
 * @param [in] len amount of bytes to erase. If more than the length of `xtr`, the
 *        entire `xtr` is emptied.
 */
XTR_API void
xtr_truncate_head(xtr_t* xtr, size_t len);

/**
 * Removes `len` bytes from the xtring end.
 *
 * Like xtr_pop_tail() without returning the popped value.
 *
 * Example of truncating 3 bytes:
 *         xtr before: "Hello world!"
 *         xtr after:  "Hello wor"
 *
 * O(1) operation.
 * @param [in,out] xtr xtring to truncate, modified in-place.
 * @param [in] len amount of bytes to erase. If more than the length of `xtr`, the
 *        entire `xtr` is emptied.
 */
XTR_API void
xtr_truncate_tail(xtr_t* xtr, size_t len);

/**
 * Trims (strips) any provided characters from the end of the string.
 *
 * Sometimes known as `rstrip` or `rtrim`.
 *
 * Example:
 *
 *         xtr_trim_tail(xtr["Hello world!"], ".,!d") --> xtr["Hello worl"]
 *         xtr_trim_tail(xtr["Hello world!\r\n"], NULL) --> xtr["Hello world!"]
 *         xtr_trim_tail(xtr["Hello world!AAAAAAA"], "ABC") --> xtr["Hello world!"]
 * @param [in,out] xtr xtring to trim in-place.
 * @param [in] chars characters to remove, regardless of their order of appearance.
 *        NULL for all whitespace characters.
 */
XTR_API void
xtr_trim_tail(xtr_t* xtr, const char* chars);

/**
 * Trims (strips) any provided characters from the start of the string.
 *
 * Sometimes known as `lstrip` or `ltrim`.
 *
 * Example:
 *
 *         xtr_trim_head(xtr["Hello world!"], "eHl") --> xtr["o world!"]
 *         xtr_trim_head(xtr["\r\n Hello world!\r\n"], NULL) --> xtr["Hello world!\r\n"]
 *         xtr_trim_head(xtr["===Hello world!"], "=-+") --> xtr["Hello world!"]
 * @param [in,out] xtr xtring to trim in-place.
 * @param [in] chars characters to remove, regardless of their order of appearance.
 *        NULL for all whitespace characters.
 */
XTR_API void  // O(n) operation!
xtr_trim_head(xtr_t* xtr, const char* chars);

/**
 * Trims (strips) any provided characters from the start and end of the string.
 *
 * It's a combination xtr_trim_head() and xtr_trim_tail().
 *
 * Example:
 *
 *         xtr_trim(xtr["Hello world!"], "eHl!") --> xtr["o world"]
 *         xtr_trim(xtr["\r\n Hello world!\r\n"], NULL) --> xtr["Hello world!"]
 *         xtr_trim(xtr["===Hello world!==="], "=-+") --> xtr["Hello world!"]
 * @param [in,out] xtr xtring to trim in-place.
 * @param [in] chars characters to remove, regardless of their order of appearance.
 *        NULL for all whitespace characters.
 */
XTR_API void
xtr_trim(xtr_t* xtr, const char* chars);

/**
 * Truncates the provided substring from the start of the xtring, if present.
 *
 * If either parameter is NULL or the prefix is not found, then nothing happens.
 * If the prefix appears multiple times, it's removed only once.
 *
 * Example:
 *         xtr_truncate_prefix(xtr["Hello world!"], "He") --> xtr["llo world!"]
 *         xtr_truncate_prefix(xtr["Hello world!"], "abc!") --> xtr["Hello world!"]
 *         xtr_truncate_prefix(xtr["...Hello world!"], ".") --> xtr["..Hello world!"]
 * @param [in,out] xtr xtring to truncate in-place. NULL does nothing.
 * @param [in] prefix substring to remove from the xtring's head, if present. NULL does nothing.
 */
XTR_API void
xtr_truncate_prefix(xtr_t* xtr, const char* prefix);

/**
 * Truncates the provided substring from the end of the xtring, if present.
 *
 * If either parameter is NULL or the suffix is not found, then nothing happens.
 * If the suffix appears multiple times, it's removed only once.
 *
 * Example:
 *         xtr_truncate_suffix(xtr["Hello world!"], " world!") --> xtr["Hello"]
 *         xtr_truncate_suffix(xtr["Hello world!"], "abc!") --> xtr["Hello world!"]
 *         xtr_truncate_suffix(xtr["Hello world..."], ".") --> xtr["Hello world.."]
 * @param [in,out] xtr xtring to truncate in-place. NULL does nothing.
 * @param [in] suffix substring to remove from the xtring's tail, if present. NULL does nothing.
 */
XTR_API void
xtr_truncate_suffix(xtr_t* xtr, const char* suffix);

// ------------------- Alter the Xtring's allocated memory ------------------------------------

XTR_API xtr_t*
xtr_ensure(xtr_t* xtr, size_t len);

XTR_API xtr_t*
xtr_ensure_free(xtr_t** pxtr, size_t len);

XTR_API xtr_t*
xtr_resize(xtr_t* xtr, size_t new_capacity);

XTR_API xtr_t*
xtr_resize_free(xtr_t** pxtr, size_t new_length);

XTR_API xtr_t*
xtr_resize_free_double(xtr_t** pxtr);

XTR_API xtr_t*
xtr_compress_free(xtr_t** pxtr);

// ------------------- Reverse the xtring's content ------------------------------------
/**
 * Creates a reversed (end-to-start) copy of the xtring.
 *
 * @param [in] xtr to reverse.
 * @return the new xtring or NULL in case of malloc failure or when `xtr` is NULL.
 */
XTR_API xtr_t*
xtr_reversed(const xtr_t* xtr);

/**
 * Reverses (end-to-start) the xtring in-place.
 *
 * @param [in,out] xtr to reverse. Does nothing on NULL input.
 */
XTR_API void
xtr_reverse(xtr_t* xtr);

// ------------------- Search for substrings ------------------------------------
/**
 * Counts how many times a substring appears in the xtring.
 *
 * @param [in] haystack xtring to search in.
 * @param [in] needle pattern to search for (substring).
 * @return amount or #XTR_NOT_FOUND in case of NULL pointers. Returns zero if
 *         `needle` is too short to fit the `haystack`.
 */
XTR_API size_t
xtr_occurrences(const xtr_t* haystack, const xtr_t* needle);

/**
 * Checks whether a substring exists in the xtring.
 *
 * @param [in] haystack xtring to search in
 * @param [in] needle pattern to search for
 * @return true if the patter is found, false otherwise. False is also returned
 *         when any pointer is NULL, if any xtring is empty or
 *         if `needle` does not fit into `haystack` at all.
 */
XTR_API bool
xtr_contains(const xtr_t* haystack, const xtr_t* needle);

/**
 * Searches for a substring in the xtring starting.
 *
 * @param [in] haystack xtring to search in
 * @param [in] needle pattern to search for
 * @return index to the found section or #XTR_NOT_FOUND if not found.
 *         #XTR_NOT_FOUND is also returned if the indexes
 *         are out of range, if any pointer is NULL, if any xtring is empty,
 *         if `needle` does not fit into `haystack` at all.
 */
XTR_API size_t
xtr_find(const xtr_t* haystack, const xtr_t* needle);

/**
 * Searches for a substring in the xtring starting from an index.
 *
 * @param [in] haystack xtring to search in
 * @param [in] needle pattern to search for
 * @param [in] start first index to check, inclusive
 * @return index to the found section or #XTR_NOT_FOUND if not found.
 *         #XTR_NOT_FOUND is also returned if the indexes
 *         are out of range, if any pointer is NULL, if any xtring is empty,
 *         if `needle` does not fit into `haystack` at all.
 */
XTR_API size_t
xtr_find_from(const xtr_t* haystack, const xtr_t* needle, size_t start);

XTR_API const size_t*
xtr_find_all(const xtr_t* haystack, const xtr_t* needle);

/**
 * Searches for a substring `needle` in the xtring within an index range.
 *
 * @param [in] haystack xtring to search in
 * @param [in] needle pattern to search for
 * @param [in] start first index to check, inclusive
 * @param [in] end end of search, excluded (points to byte after last)
 * @return index to the found section or #XTR_NOT_FOUND if not found.
 *         #XTR_NOT_FOUND is also returned if the indexes
 *         are out of range, if any pointer is NULL, if any xtring is empty,
 *         if `needle` does not fit into `haystack` at all.
 */
XTR_API size_t
xtr_find_within(const xtr_t* haystack, const xtr_t* needle, size_t start, size_t end);

// ------------------- Splitting ------------------------------------

XTR_API xtr_t**
xtr_split(size_t* amount_of_chunks, const xtr_t* xtr, const xtr_t* separator);

XTR_API xtr_t**
xtr_split_every(size_t* amount_of_chunks, const xtr_t* xtr, size_t chunk_len);

// ------------------- Concatenation ------------------------------------
/**
 * Concatenates two xtrings into a third one.
 *
 * Can be also used to concatenate `a` with itself.
 * @param [in] a first half.
 * @param [in] b second half. May also be `a`.
 * @return the new xtring containing `a+b` or NULL in case of malloc failure,
 *         NULL inputs or size overflow.
 */
XTR_API xtr_t*
xtr_concat(const xtr_t* a, const xtr_t* b);

/**
 * New xtring with the content repeated `repetition` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param [in] xtr xtring to repeat.
 * @param [in] repetitions amount of times to repeat `xtr`. `0` for an empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_repeated(const xtr_t* xtr, size_t repetitions);

// ------------------- Appending ------------------------------------

/**
 * Appends the extension to the existing xtring's start, if there is enough capacity.
 * Otherwise does nothing.
 *
 * If there is some capacity but not enough for the
 * full extension, it also does nothing - no partial copy is performed.
 *
 * @param [in,out] xtr xtring to extend
 * @param [in] extension xtring to add as extension
 * @return Amount of bytes extended. Zero if any pointer is NULL or if `xtr` has not
 *         enough space for the full extension.
 */
XTR_API size_t
xtr_push_head(xtr_t* xtr, const xtr_t* extension);

/**
 * Appends the extension to the existing xtring's end, if there is enough capacity.
 * Otherwise does nothing.
 *
 * If there is some capacity but not enough for the
 * full extension, it also does nothing - no partial copy is performed.
 *
 * @param [in,out] xtr xtring to extend
 * @param [in] extension xtring to add as extension
 * @return Amount of bytes extended. Zero if any pointer is NULL or if `xtr` has not
 *         enough space for the full extension.
 */
XTR_API size_t
xtr_push_tail(xtr_t* xtr, const xtr_t* extension);

/**
 * Appends the extension to the existing xtring's start, reallocating the xtring
 * if necessary, freeing the old one.
 *
 * @param [in,out] pxtr pointer to the original xtring to extend. Pointed xtr_t*
 *        will be replaced by a larger copy, if a reallocation happens.
 *        Frees the previous xtring. Untouched on failure.
 * @param [in] extension xtring to add as extension
 * @return the new xtring or NULL in case of malloc failure or when `pxtr` or `*pxtr` is NULL.
 *         Note: on success the returned value matches `*pxtr`. On failure the
 *         returned value is NULL and *pxtr is unchanged.
 */
XTR_API xtr_t*
xtr_extend_head(xtr_t** pxtr, const xtr_t* extension);

/**
 * Appends the extension to the existing xtring's end, reallocating the xtring
 * if necessary, freeing the old one.
 *
 * @param [in,out] pxtr pointer to the original xtring to extend. Pointed xtr_t*
 *        will be replaced by a larger copy, if a reallocation happens.
 *        Frees the previous xtring. Untouched on failure.
 * @param [in] extension xtring to add as extension
 * @return the new xtring or NULL in case of malloc failure or when `pxtr` or `*pxtr` is NULL.
 *         Note: on success the returned value matches `*pxtr`. On failure the
 *         returned value is NULL and *pxtr is unchanged.
 */
XTR_API xtr_t*
xtr_extend_tail(xtr_t** pxtr, const xtr_t* extension);

// ------------------- Encoding ------------------------------------
/**
 * Converts a binary xtring to a hex string in ASCII encoding.
 *
 * Example: xtr[13, 1, 130] --> "0D0182"
 *
 * @param [in] bin xtring to convert to hex
 * @param [in] upper true to use uppercase hex characters ABCDEF rather than abcdef
 * @param [in] separator optional string to place between each byte. NULL or empty
 *        string for no separator
 * @return a new xtring with the hex characters in ASCII encoding or NULL in
 *         case of malloc failure.
 */
XTR_API xtr_t*
xtr_to_hex(const xtr_t* bin, bool upper, const char* separator);

/**
 * Converts a hexadeciaml text string in ASCII encoding to a binary xtring.
 *
 * Example: "0x0D 01 82" --> xtr[13, 1, 130]s
 *
 * Properties:
 * - Ignores any whitespaces, comma, and underscores - typically used as byte
 *   separators.
 * - Ignores any `#` sign - typically used when expressing RGB colors as hex.
 * - Ignores any `0x` or `0X` prefixes, even if they appear multiple times.
 * - Case insensitive (A-F or a-f are both OK).
 *
 * @param [in] hex hexadecimal string (ASCII characters 0-9A-F)
 * @param [in] len optional length of the hex string, if already known.
 *             Pass #XTR_UNKNOWN_STRLEN otherwise: this function will run strlen() internally.
 * @return a new xtring with the binary values or NULL in case of conversion failure (e.g.,
 * non-hex character found) or malloc failure
 */
XTR_API xtr_t*
xtr_from_hex(const char* hex, size_t len);

XTR_API xtr_t*
xtr_base64_decode(const xtr_t* b64_text);
XTR_API xtr_t*
xtr_base64_encode(const xtr_t* binary);

// ------------------- Utils ------------------------------------
XTR_API const char*
xtr_api_version(uint32_t* version);

#if defined(XTR_STDIO) && XTR_STDIO

XTR_API void
xtr_puts(const xtr_t* xtr);
#endif

#ifdef __cplusplus
}
#endif

#endif /* XTR_H */
