# Copyright © 2022-2024, Matjaž Guštin <dev@matjaz.it>
# <https://matjaz.it>. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of nor the names of its contributors may be used to
#    endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS “AS IS”
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# CMake configuration to build Xtr
# For example usage: see README.md

cmake_minimum_required(VERSION 3.9)
project(Xtr
        VERSION 0.1.0
        LANGUAGES C
        DESCRIPTION
        "Extendable strings for C")

# Default build type, if not specified explicitly with `-DCMAKE_BUILD_TYPE`
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
    message("CMAKE_BUILD_TYPE unspecified, defaulting to ${CMAKE_BUILD_TYPE}")
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# Compiler flags
# -----------------------------------------------------------------------------
include(CheckIncludeFile)
CHECK_INCLUDE_FILE("assert.h" ASSERT_H_EXISTS)
if (ASSERT_H_EXISTS AND (CMAKE_BUILD_TYPE EQUAL "Debug"))
    message(STATUS "assert.h found. Enabling runtime assertions verifying "
            "the library API inputs using assert() but only in debug mode "
            "for all targets.")
    add_compile_definitions(XTR_INPUT_ASSERTS=1)
endif ()
CHECK_INCLUDE_FILE("stdio.h" STDIO_H_EXISTS)
if (STDIO_H_EXISTS)
    message(STATUS "stdio.h found. Enabling printing functions for all targets.")
    add_compile_definitions(XTR_STDIO=1)
endif ()


# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
set(XTR_SRC
        src/xtr_clone.c
        src/xtr_cmp.c
        src/xtr_decrease.c
        src/xtr_get.c
        src/xtr_hex.c
        src/xtr_increase.c
        src/xtr_internal.h
        src/xtr_new.c
        src/xtr_resize.c
        src/xtr_reverse.c
        src/xtr_search.c
        src/xtr_split.c
        src/xtr_unicode.c
        src/xtr_utils.c
        src/xtr_from.c
        src/xtr_unarycmp.c
        src/xtr_random.c
        src/xtr_base64.c
        src/xtr_print.c)
set(XTRTEST_SRC
        # Test frameworks
        tst/framework/atto/atto.c
        tst/framework/xtrtest_main.c
        tst/framework/xtrtest_malloc.c
        # Test cases
        tst/xtrtest_new.c
        tst/xtrtest_new_with_capacity.c
        tst/xtrtest_zeros.c
        tst/xtrtest_free.c
        tst/xtrtest_getters.c
        tst/xtrtest_from_str.c
        tst/xtrtest_from_str_with_capacity.c
        tst/xtrtest_from_str_repeated.c
        tst/xtrtest_from_str_repeated_with_capacity.c
        tst/xtrtest_clone.c
        tst/xtrtest_clone_with_capacity.c
        tst/xtrtest_is_empty.c
        tst/xtrtest_is_spaces.c
)


# -----------------------------------------------------------------------------
# Build targets
# -----------------------------------------------------------------------------
# Copy the library API header into the build directory.
# With this command, if the header changes, it's copied, otherwise not.
configure_file(inc/xtr.h ${CMAKE_BINARY_DIR} COPYONLY)

# Static libraries (.a)
add_library(xtr_static STATIC ${XTR_SRC})
target_include_directories(xtr_static PUBLIC inc/ PRIVATE src/)
set_target_properties(xtr_static PROPERTIES
        INTERPROCEDURAL_OPTIMISATION TRUE
        PREFIX lib
        OUTPUT_NAME xtr)
if (WIN32 OR MSYS)
    target_link_libraries(xtr_static PRIVATE bcrypt)
endif ()

# Shared library (.so / .dylib / .dll)
# Does not reuse the static library object files, as they are
# recompiled in order to have position-independent code
add_library(xtr SHARED ${XTR_SRC})
target_include_directories(xtr PUBLIC inc/ PRIVATE src/)
set_target_properties(xtr PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        INTERPROCEDURAL_OPTIMISATION TRUE
        # Remove any "msys-" and enforce the same lib name with all toolchains
        PREFIX lib
        OUTPUT_NAME xtr)
if (WIN32 OR MSYS)
    target_link_libraries(xtr PRIVATE bcrypt)
endif ()

enable_testing()
# Test runner executable testing the static library
add_executable(xtrtest_static ${XTRTEST_SRC})
target_include_directories(xtrtest_static
        PUBLIC inc/
        PRIVATE tst/framework/
        PRIVATE tst/framework/atto/)
add_dependencies(xtrtest_static xtr_static)
target_link_libraries(xtrtest_static PUBLIC xtr_static)
set_target_properties(xtrtest_static PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        INTERPROCEDURAL_OPTIMISATION TRUE)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_definitions(xtrtest_static PRIVATE XTRTEST_WRAP_MALLOC=1)
    target_link_options(xtrtest_static PUBLIC
            -Wl,--wrap=malloc
            -Wl,--wrap=calloc
            -Wl,--wrap=realloc)
endif ()
add_test(COMMAND xtrtest_static TARGET xtrtest_static)

# Test runner executable testing the shared library
add_executable(xtrtest ${XTRTEST_SRC})
target_include_directories(xtrtest
        PUBLIC inc/
        PRIVATE tst/framework/
        PRIVATE tst/framework/atto/)
add_dependencies(xtrtest xtr)
target_link_libraries(xtrtest PUBLIC xtr)
set_target_properties(xtrtest PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        INTERPROCEDURAL_OPTIMISATION TRUE)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_definitions(xtrtest_static PRIVATE XTRTEST_WRAP_MALLOC=1)
    target_link_options(xtrtest_static PUBLIC
            -Wl,--wrap=malloc
            -Wl,--wrap=calloc
            -Wl,--wrap=realloc)
endif ()
add_test(COMMAND xtrtest TARGET xtrtest)

# Doxygen documentation builder
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (DOXYGEN_FOUND)
    # Cmake's wrapper of Doxygen, constructing a doxyfile from the
    # DOXYGEN_* variables, which are mapped to the Doxygen variables.

    # Parts of the source documentation to work on
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE NO)
    set(DOXYGEN_EXTRACT_PRIV_VIRTUAL NO)
    set(DOXYGEN_EXTRACT_PACKAGE NO)
    set(DOXYGEN_EXTRACT_STATIC NO)
    set(DOXYGEN_EXTRACT_LOCAL_CLASSES NO)
    set(DOXYGEN_EXTRACT_LOCAL_METHODS NO)
    set(DOXYGEN_EXTRACT_ANON_NSPACES NO)
    set(DOXYGEN_INTERNAL_DOCS NO)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

    # How to process the source code
    set(DOXYGEN_INPUT_ENCODING UTF-8)
    set(DOXYGEN_BRIEF_MEMBER_DESC YES)
    set(DOXYGEN_REPEAT_BRIEF YES)
    set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
    set(DOXYGEN_MARKDOWN_SUPPORT YES)
    set(DOXYGEN_TAB_SIZE 4)

    # Components and look of the output
    set(DOXYGEN_OUTPUT_LANGUAGE English)
    set(DOXYGEN_TOC_INCLUDE_HEADINGS 5)
    set(DOXYGEN_AUTOLINK_SUPPORT YES)
    set(DOXYGEN_HIDE_UNDOC_MEMBERS NO)
    set(DOXYGEN_HIDE_UNDOC_CLASSES NO)
    set(DOXYGEN_HIDE_IN_BODY_DOCS NO)
    set(DOXYGEN_SORT_MEMBER_DOCS NO)
    set(DOXYGEN_SORT_BRIEF_DOCS NO)
    set(DOXYGEN_MAX_INITIALIZER_LINES 30)
    #set(DOXYGEN_PROJECT_LOGO )

    # Format of the output
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN YES)

    # Processing
    set(DOXYGEN_NUM_PROC_THREADS 0)  # As many as CPU cores
    set(DOXYGEN_QUIET YES)
    set(DOXYGEN_WARNINGS YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_WARN_NO_PARAMDOC YES)
    set(DOXYGEN_WARN_AS_ERROR YES)
    if (DOT_FOUND)
        set(DOXYGEN_DOT_PATH) # Empty = find it in PATH
        set(DOXYGEN_DOT_NUM_THREADS 0)  # As many as CPU cores
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_DIRECTORY_GRAPH YES)
    endif ()

    # Customisations
    set(DOXYGEN_ALIASES license="**License:**")
    set(DOXYGEN_PREDEFINED WIN32 XTR_INPUT_ASSERTS)

    doxygen_add_docs(xtr_doxygen
            # Do NOT build doxygen on make-all, to avoid polluting the stdout
            # List of input files for Doxygen
            ${PROJECT_SOURCE_DIR}/inc/xtr.h
            ${PROJECT_SOURCE_DIR}/LICENSE.md
            ${PROJECT_SOURCE_DIR}/README.md
            ${PROJECT_SOURCE_DIR}/CHANGELOG.md
            COMMENT "Generating Doxygen documentation...")
else (DOXYGEN_FOUND)
    message(WARNING "Doxygen not found. Cannot generate documentation.")
endif (DOXYGEN_FOUND)
