/**
* @file
 * Tests run by the xtrtest_main/main().
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

#ifndef XTRTEST_H
#define XTRTEST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "atto.h"
#include "xtr.h"

// Test framework functions
void
xtrtest_malloc_fail_after(size_t count);
void
xtrtest_malloc_disable_failing(void);

// clang-format off
// @formatter: off
// BEGIN OF AUTOMATED LISTING OF ALL XTRTEST TESTCASES
void xtrtest_clone_valid_1_char_xtr(void);
void xtrtest_clone_valid_empty_xtr(void);
void xtrtest_clone_validaaaa_6_char_xtr(void);
void xtrtest_clone_with_capacity_valid_1_char_xtr_less_capacity(void);
void xtrtest_clone_with_capacity_valid_1_char_xtr_more_capacity(void);
void xtrtest_clone_with_capacity_valid_1_char_xtr_same_capacity(void);
void xtrtest_clone_with_capacity_valid_empty_xtr_more_capacity(void);
void xtrtest_clone_with_capacity_valid_empty_xtr_same_capacity(void);
void xtrtest_free_valid(void);
void xtrtest_free_valid_on_null_input(void);
void xtrtest_from_str_fail_malloc(void);
void xtrtest_from_str_repeated_fail_malloc(void);
void xtrtest_from_str_repeated_valid_1_byte_string_0_times(void);
void xtrtest_from_str_repeated_valid_1_byte_string_1_times(void);
void xtrtest_from_str_repeated_valid_1_byte_string_2_times(void);
void xtrtest_from_str_repeated_valid_6_bytes_string_3_times(void);
void xtrtest_from_str_repeated_valid_empty_string(void);
void xtrtest_from_str_repeated_valid_empty_string_0_times(void);
void xtrtest_from_str_repeated_valid_null_0_times(void);
void xtrtest_from_str_repeated_valid_null_1_time(void);
void xtrtest_from_str_repeated_with_capacity_fail_malloc(void);
void xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_0_times_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_0_times_7_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_1_times_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_1_times_7_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_2_times_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_1_byte_string_2_times_7_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_6_bytes_string_3_times_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_6_bytes_string_3_times_20_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_times_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_empty_string_0_times_7_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_empty_string_7_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_null_0_times_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_null_0_times_7_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_null_1_time_0_space(void);
void xtrtest_from_str_repeated_with_capacity_valid_null_1_time_7_space(void);
void xtrtest_from_str_valid_1_byte_string(void);
void xtrtest_from_str_valid_6_bytes_string(void);
void xtrtest_from_str_valid_empty_string(void);
void xtrtest_from_str_valid_null(void);
void xtrtest_from_str_with_capacity_fail_malloc(void);
void xtrtest_from_str_with_capacity_valid_1_char_0_bytes(void);
void xtrtest_from_str_with_capacity_valid_1_char_1_bytes(void);
void xtrtest_from_str_with_capacity_valid_1_char_2_bytes(void);
void xtrtest_from_str_with_capacity_valid_6_chars_10_bytes(void);
void xtrtest_from_str_with_capacity_valid_6_chars_4_bytes(void);
void xtrtest_from_str_with_capacity_valid_6_chars_6_bytes(void);
void xtrtest_from_str_with_capacity_valid_empty_string_0_bytes(void);
void xtrtest_from_str_with_capacity_valid_empty_string_15_bytes(void);
void xtrtest_from_str_with_capacity_valid_empty_string_1_byte(void);
void xtrtest_from_str_with_capacity_valid_null_string_0_bytes(void);
void xtrtest_from_str_with_capacity_valid_null_string_6_bytes(void);
void xtrtest_getters_do_nothing_on_null_input(void);
void xtrtest_is_empty_valid_empty(void);
void xtrtest_is_empty_valid_empty_with_capacity(void);
void xtrtest_is_empty_valid_non_empty(void);
void xtrtest_is_empty_valid_null(void);
void xtrtest_is_spaces_valid_different_whitespaces(void);
void xtrtest_is_spaces_valid_empty(void);
void xtrtest_is_spaces_valid_empty_with_capacity(void);
void xtrtest_is_spaces_valid_many_spaces(void);
void xtrtest_is_spaces_valid_non_spaces(void);
void xtrtest_is_spaces_valid_not_only_whitespaces(void);
void xtrtest_is_spaces_valid_null(void);
void xtrtest_is_spaces_valid_single_space(void);
void xtrtest_new_empty_fail_malloc(void);
void xtrtest_new_empty_valid(void);
void xtrtest_new_ensure_fail_size_overflow(void);
void xtrtest_new_with_capacity_fail_malloc(void);
void xtrtest_new_with_capacity_fail_size_overflow(void);
void xtrtest_new_with_capacity_valid_allocate_0_bytes(void);
void xtrtest_new_with_capacity_valid_allocate_15_bytes(void);
void xtrtest_new_with_capacity_valid_allocate_1_byte(void);
void xtrtest_new_with_capacity_valid_allocate_ffff_plus_1_bytes(void);
void xtrtest_zeros_fail_malloc(void);
void xtrtest_zeros_valid_1_byte(void);
void xtrtest_zeros_valid_6_bytes(void);
void xtrtest_zeros_valid_empty(void);
// END OF AUTOMATED LISTING OF ALL XTRTEST TESTCASES
// clang-format on
// @formatter: on

#ifdef __cplusplus
}
#endif

#endif  /* XTRTEST_H */
