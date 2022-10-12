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

// Assuming enough memory
#define XTR_MAX_CAPACITY (SIZE_MAX - sizeof(size_t) * 2U - 1U)

#define XTR_UNKNOWN_STRLEN SIZE_MAX

/** Opaque xtring structure.
 * Internally it may preallocate more space than it requires.
 *
 *             capacity (buffer size)
 *             __________________
 *            /                  \
 *           [abcde...............] buffer
 *            \___/\_____________/
 *           length       available
 *         (used space)  (free space)
 */
typedef struct xtr xtr_t;

/**
 * If true, enables zeroing-out of the heap memory on allocation and before
 * freeing it.
 *
 * Recommended for higher memory safety assurance, at the cost of additional
 * O(n) operation on construction and destruction.
 */
#define XTR_CLEAR_HEAP 1

// =================== NEW XTRINGS ============================================
// ------------------- New empty xtrings ------------------------------------------
/**
 * Allocates an empty xtring with no additional pre-allocated free space.
 *
 * Equivalent to `xtr_new_with_capacity(0)`.
 *
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_new(void);

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
xtr_new_with_capacity(size_t capacity);

/**
 * Frees the allocated memory of xtring after usage and sets the xtring pointer
 * to NULL to avoid use-after-free.
 *
 * Example:
 *         xtr_t* xtring = xtr_new();
 *         // ... usage of the xtring
 *         xtr_free(&xtring);
 *         assert(xtring == NULL);
 *
 * @param pxtr **address** of the xtring-pointer.
 */
XTR_API void
xtr_free(xtr_t** pxtr);

// ------------------- New initialised xtrings from other data ------------------------------------
/**
 * New xtring filled with zero-values bytes, similar to `calloc`.
 *
 * @param len length in bytes = amount of zeros. `0` for empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_zeros(size_t len);

/**
 * New xtring initialised with the C-string content.
 *
 * Does not reserve any additional capacity for expansions.
 * @param str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str(const char* str);
// TODO consider providing an OPTIONAL string length field so the function
// does not compute strlen() again, if avaialable. Use SIZE_MAX as default,
// which is anyway unsupported, as the xtring would not fit a SIZE_MAX string
// given the struct overhead.

// TODO from str/u8-array UNTIL a limit (like strncpy with len < strlen())

/**
 * New xtring initialised with the C-string content and overall `at_least` allocated
 * space.
 *
 * Ensures the `at_least - strlen(str)` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @param at_least minimum amount of bytes to allocate, but
 *        `strlen(str)` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str_with_capacity(const char* str, size_t at_least);

/**
 * New xtring initialised with the C-string content repeated `repetitions` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @param repetitions amount of times to repeat `str`. `0` for an empty xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str_repeated(const char* str, size_t repetitions);

/**
 * New xtring initialised with the C-string content repeated `repetitions` times
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - strlen(str)` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param str null-terminated array of characters, usually ASCII.
 *        NULL or `""` for an empty xtring.
 * @param repetitions amount of times to repeat `str`. `0` for an empty xtring.
 * @param at_least minimum amount of bytes to allocate, but
 *        `strlen(str) * repetitions` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_str_repeated_with_capacity(const char* str,
                                    size_t repetitions,
                                    size_t at_least);

/**
 * New xtring initialised with the array content.
 *
 * Does not reserve any additional capacity for expansions.
 * @param array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param array_len amount of bytes to copy from the array.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_array(const uint8_t* array,
               size_t array_len);

/**
 * New xtring initialised with the array content and overall `at_least` allocated
 * space.
 *
 * Ensures the `at_least - array_len` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param array_len amount of bytes to copy from the array.
 * @param at_least minimum amount of bytes to allocate, but
 *        `array_len` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_array_with_capacity(const uint8_t* array,
                             size_t array_len,
                             size_t at_least);

/**
 * New xtring initialised with the array content repeated `repetitions` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param array_len amount of bytes to copy from the array.
 * @param repetitions amount of times to repeat `array`.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_array_repeated(const uint8_t* array,
                        size_t array_len,
                        size_t repetitions);

/**
 * New xtring initialised with the C-string content repeated `repetitions` times
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - array_len` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 * @param array binary array. Zero-bytes are copied as they are, not interpreted
 *        as null-terminators.
 * @param array_len amount of bytes to copy from the array.
 * @param repetitions amount of times to repeat `array`.
 * @param at_least minimum amount of bytes to allocate, but
 *        `array_len * repetitions` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_array_repeated_with_capacity(const uint8_t* array,
                                      size_t array_len,
                                      size_t repetitions,
                                      size_t at_least);

/**
 * New xtring of length 1 initialised with a single byte.
 *
 * Does not reserve any additional capacity for expansions.
 * @param byte 8-bit value to place in the xtring.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_byte(uint8_t byte);

/**
 * New xtring initialised with the `byte` repeated `repetitions` times.
 *
 * Does not reserve any additional capacity for expansions.
 * @param byte 8-bit value to place in the xtring.
 * @param repetitions amount of times to repeat `byte`.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_byte_repeated(uint8_t byte, size_t repetitions);

/**
 * New xtring initialised with the `byte` repeated `repetitions` times
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - repetitions` available allocated free space at the
 * xtring's end, to have some space ready for expansions without reallocation.
 *
 * Does not reserve any additional capacity for expansions.
 * @param byte 8-bit value to place in the xtring.
 * @param repetitions amount of times to repeat `byte`.
 * @param at_least minimum amount of bytes to allocate, but
 *        `repetitions` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_from_byte_repeated_with_capacity(uint8_t byte, size_t len, size_t at_least);

/**
 * New xtring initialised with the `len` cryptographically-secure random bytes.
 *
 * On Windows it obtains the random bytes using `BCryptGenRandom`, on
 * Unix-like systems it uses `/dev/urandom`.
 *
 * Does not reserve any additional capacity for expansions.
 * @param len amount of random bytes.
 * @return the new xtring or NULL in case of malloc failure.
 */
XTR_API xtr_t*
xtr_random(size_t len);

// ------------------- New cloned xtrings ------------------------------------
/**
 * New xtring with same content as another one (copy by value).
 *
 * @param xtr to copy.
 * @return the new xtring or NULL in case of malloc failure or when `xtr` is NULL.
 */
XTR_API xtr_t*
xtr_clone(const xtr_t* xtr);

/**
 * New xtring with same content as another one (copy by value)
 * and overall `at_least` allocated space.
 *
 * Ensures the `at_least - xtr_len(xtr)` available allocated free space at the
 * clone's end, to have some space ready for expansions without reallocation.
 * @param xtr to copy. Returns NULL if `xtr` is NULL.
 * @param at_least minimum amount of bytes to allocate, but
 *        `xtr_len(xtr)` is anyhow allocated not to truncate any data.
 * @return the new xtring or NULL in case of malloc failure or when `xtr` is NULL.
 */
XTR_API xtr_t*
xtr_clone_with_capacity(const xtr_t* xtr, size_t max_len);


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
 * @param xtr xtring to inspect.
 * @return the length or 0 if `xtr` is NULL.
 */
XTR_API size_t
xtr_len(const xtr_t* xtr);

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
 * @param xtr xtring to inspect.
 * @return the capacity or 0 if `xtr` is NULL.
 */
XTR_API size_t
xtr_capacity(const xtr_t* xtr);

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
 * @param xtr xtring to inspect.
 * @return the capacity or 0 if `xtr` is NULL.
 */
XTR_API size_t
xtr_available(const xtr_t* xtr);

/**
 * Read-only access to the internal buffer as C-string, null-terminated.
 *
 * DO NOT USE IT TO WRITE DATA. It will corrupt the internal structure.
 *
 * @param xtr xtring to inspect.
 * @return null-terminated char array or NULL if `xtr` is NULL.
 */
XTR_API const char*
xtr_cstring(const xtr_t* xtr);

/**
 * Read-only access to the internal buffer as `uint8_t[]`, null-terminated.
 *
 * DO NOT USE IT TO WRITE DATA. It will corrupt the internal structure.
 *
 * Although the null-termination is not required for binary arrays, it's
 * included due to the internal construction.
 * @param xtr xtring to inspect.
 * @return null-terminated `uint8_t` array or NULL if `xtr` is NULL.
 */
XTR_API const uint8_t*
xtr_array(const xtr_t* xtr);

/**
 * Read-only pointer to the last byte of the string.
 *
 * @param xtr xtring to inspect.
 * @return pointer or NULL if `xtr` is NULL or empty.
 */
XTR_API const uint8_t*
xtr_last(const xtr_t* xtr);

// ------------------- Single-Xtrings content analysis ------------------------------------

/**
 * Checks whether the xtring contains any bytes.
 * @param xtr xtring to inspect.
 * @return true if `xtr` is empty (length == 0) or NULL, false otherwise.
 */
XTR_API bool
xtr_is_empty(const xtr_t* xtr);

/**
 * Checks whether the xtring contains only whitespace characters.
 * @param xtr xtring to inspect.
 * @return true if `xtr` contains only `isspace()` characters, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_spaces(const xtr_t* xtr);

/**
 * Checks whether the xtring contains only zero-valued bytes.
 * @param xtr xtring to inspect.
 * @return true if `xtr` contains only zeros, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_zeros(const xtr_t* xtr);

/**
 * Like xtr_is_zeros() but with constant runtime for security application.
 *
 * The entire string is always scanned, even if a non-zero byte is found early.
 * @param xtr xtring to inspect.
 * @return true if `xtr` contains only zeros, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_zeros_consttime(const xtr_t* xtr);

/**
 * Checks whether the xtring is not full of just zero-valued bytes.
 * @param xtr xtring to inspect.
 * @return true if `xtr` contains any non-zero byte, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_not_zeros(const xtr_t* xtr);

/**
 * Like xtr_is_not_zeros() but with constant runtime for security application.
 *
 * The entire string is always scanned, even if a non-zero byte is found early.
 * @param xtr xtring to inspect.
 * @return true if `xtr` contains any non-zero byte, false otherwise.
 *         Note: an empty or NULL xtring returns false.
 */
XTR_API bool
xtr_is_not_zeros_consttime(const xtr_t* a);

// ------------------- Xtring equality check and comparison ------------------------------------

/**
 * Compares just the lengths of the xtrings (amount of used bytes).
 *
 * Mimics `memcmp()` but just for the xtring lengths, not their content.
 * For that, use xtr_cmp().
 * @param a first xtring
 * @param b second xtring
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
 * @param a first xtring
 * @param b second xtring
 * @return true when both are NULL or have equal length
 */
XTR_API bool
xtr_is_equal_length(const xtr_t* a, const xtr_t* b);

/**
 * Compares the two xtrings in length and content, useful for sorting.
 *
 * Mimics `memcmp()`, but compares also the length and NULL pointers.
 * @param a first xtring
 * @param b second xtring
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
 * @param a first xtring
 * @param b second xtring
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
 * @param a first xtring
 * @param b second xtring
 * @return true when both NULL, or both are the same pointer, or both have the same length
 * and same content (used bytes). False otherwise.
 */
XTR_API bool
xtr_is_equal_consttime(const xtr_t* a, const xtr_t* b);

// ------------------- Shorten the Xtring's content ------------------------------------
/**
 * Empties the string, cutting its length to zero, without reallocation.
 *
 * The allocated memory (capability) remains the same. To shorten the buffer
 * use xtr_compress_free();
 * @param xtr xtring to erase.
 */
XTR_API void // TODO if safe, then O(n) operation
xtr_clear(xtr_t* xtr);

/**
 * Removes `len` bytes from the xtring start and returns them as a new xtring.
 *
 * Example of popping 3 bytes:
 *         xtr before: "Hello world!"
 *         xtr after:  "lo world!"
 *         returned:   "Hel"
 *
 * O(n) operation, as it requires the rest of the altered string to be
 * shifted with `memmove`.
 * @param xtr xtring to pop from an alter, modified in-place.
 * @param len amount of bytes to pop. If more than the length of `xtr`, a copy
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
 * @param xtr xtring to pop from an alter, modified in-place.
 * @param len amount of bytes to pop. If more than the length of `xtr`, a copy
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
 * @param xtr xtring to truncate, modified in-place.
 * @param len amount of bytes to erase. If more than the length of `xtr`, the
 *        entire `xtr` is emptied.
 */
XTR_API void
xtr_truncate_head(xtr_t* xtr, size_t len);

// TODO indicate complexities with safe on and off

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
 * @param xtr xtring to truncate, modified in-place.
 * @param len amount of bytes to erase. If more than the length of `xtr`, the
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
 * @param xtr xtring to trim in-place.
 * @param chars characters to remove, regardless of their order of appearance.
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
 * @param xtr xtring to trim in-place.
 * @param chars characters to remove, regardless of their order of appearance.
 *        NULL for all whitespace characters.
 */
XTR_API void // O(n) operation!
xtr_trim_head(xtr_t* xtr, const char* chars); // Pass NULL for whitespaces

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
 * @param xtr xtring to trim in-place.
 * @param chars characters to remove, regardless of their order of appearance.
 *        NULL for all whitespace characters.
 */
XTR_API void
xtr_trim(xtr_t* xtr, const char* chars); // Pass NULL for whitespaces

/**
 * Truncates the provided substring from the end of the xtring, if present.
 *
 * If either parameter is NULL or the suffix is not found, then nothing happens.
 * If the suffix appears multiple times, it's removed only once.
 *
 * Example:
 *         xtr_trim(xtr["Hello world!"], " world!") --> xtr["Hello"]
 *         xtr_trim(xtr["Hello world!"], "abc!") --> xtr["Hello world!"]
 *         xtr_trim(xtr["Hello world..."], ".") --> xtr["Hello world.."]
 * @param xtr xtring to truncate in-place. NULL does nothing.
 * @param chars substring to remove. NULL does nothing.
 */
XTR_API void
xtr_remove_suffix(xtr_t* xtr, const char* suffix);

/**
 * Truncates the provided substring from the start of the xtring, if present.
 *
 * If either parameter is NULL or the prefix is not found, then nothing happens.
 * If the prefix appears multiple times, it's removed only once.
 *
 * Example:
 *         xtr_trim(xtr["Hello world!"], "He") --> xtr["llo world!"]
 *         xtr_trim(xtr["Hello world!"], "abc!") --> xtr["Hello world!"]
 *         xtr_trim(xtr["...Hello world!"], ".") --> xtr["..Hello world!"]
 * @param xtr xtring to truncate in-place. NULL does nothing.
 * @param chars substring to remove. NULL does nothing.
 */
XTR_API void
xtr_remove_prefix(xtr_t* xtr, const char* prefix);


// ------------------- Alter the Xtring's allocated memory ------------------------------------

XTR_API xtr_t*
xtr_resize(xtr_t* xtr, size_t len);

XTR_API xtr_t*
xtr_resize_double(xtr_t* xtr);

XTR_API xtr_t*
xtr_resize_free(xtr_t** pxtr, size_t len);

XTR_API xtr_t*
xtr_resize_free_double(xtr_t** pxtr);

XTR_API xtr_t*
xtr_compress_free(xtr_t** pxtr);


// ------ OTHER
XTR_API xtr_t*
xtr_reversed(const xtr_t* xtr);

XTR_API void
xtr_reverse(xtr_t* xtr);

XTR_API size_t
xtr_occurrences(const xtr_t* xtr, const xtr_t* pattern);

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

// ------------------- Encoding ------------------------------------
XTR_API xtr_t*
xtr_to_hex(const xtr_t* bin, bool upper, const char* separator);

XTR_API xtr_t*
xtr_from_hex(const char* hex, size_t len);
// Skips whitespace, commas, # (used in colours), underscores, 0x, 0X


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
