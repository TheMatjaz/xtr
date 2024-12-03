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

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
message(STATUS "C compiler ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "C compiler version: ${CMAKE_C_COMPILER_VERSION}")

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using compiler flags for Clang")

    # Activate a million warnings to have the cleanest possible code
    string(CONCAT CMAKE_C_FLAGS " -Weverything") # Activate ALL warnings

    # NOTE: deactivating padded struct warning: all structs in LibAscon have been inspected and
    # optimised. Some will still inevitably have trailing padding (to reach the aligned address
    # where the next struct in an array of structs would start) and this padding varies on each
    # platform. There is not much to be done anymore.
    #string(CONCAT CMAKE_C_FLAGS "-Wpadded")  # Struct contains paddings

    # NOTE: deactivating goto warnings: they are used only within the test suite, not the library
    #string(CONCAT CMAKE_C_FLAGS "-Wjump-misses-init")  # Switch/goto jump skips variable init

    # Debug mode
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -g3")  # Max debug info
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -O0")  # Do not optimise
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -coverage")  # Gather code coverage info
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -static-libasan")

    # Release mode optimised for speed
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -O3")  # Max optimisation for speed
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -Werror")  # Warnings as errors
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -fomit-frame-pointer")
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -march=native")
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -mtune=native")
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -funroll-loops")

    # Release mode optimised for size
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -Os")  # Optimise for size
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -fomit-frame-pointer")
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -march=native")
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -mtune=native")

elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using compiler flags for GCC")

    # Activate a million warnings to have the cleanest possible code
    string(CONCAT CMAKE_C_FLAGS " -Wall -Wextra -pedantic") # Activate most warnings
    string(CONCAT CMAKE_C_FLAGS " -Wconversion")  # Values are implicitly converted
    string(CONCAT CMAKE_C_FLAGS " -Wsign-conversion")  # Signed to/from unsigned implicit conversion
    string(CONCAT CMAKE_C_FLAGS " -Wdouble-promotion")  # Floats implicitly promoted to doubles
    string(CONCAT CMAKE_C_FLAGS " -Wfloat-equal")  # Floats compared exactly instead of approx.
    string(CONCAT CMAKE_C_FLAGS " -Wswitch-default")  # Switch-case missing default
    string(CONCAT CMAKE_C_FLAGS " -Wswitch-enum")  # Switch-case of an enum not covering all values
    string(CONCAT CMAKE_C_FLAGS " -Wuninitialized")  # Usage of uninitialised variable
    string(CONCAT CMAKE_C_FLAGS " -Wno-unused-variable")  # Variable never used
    string(CONCAT CMAKE_C_FLAGS " -Wpacked")  # Packing of struct not needed

    # NOTE: deactivating padded struct warning: all structs in LibAscon have been inspected and
    # optimised. Some will still inevitably have trailing padding (to reach the aligned address
    # where the next struct in an array of structs would start) and this padding varies on each
    # platform. There is not much to be done anymore.
    #string(CONCAT CMAKE_C_FLAGS "-Wpadded")  # Struct contains paddings

    string(CONCAT CMAKE_C_FLAGS " -Wshadow")  # Shadowing variable name
    string(CONCAT CMAKE_C_FLAGS " -Waggregate-return")  # Returning a struct from a function
    string(CONCAT CMAKE_C_FLAGS " -Wformat-security")  # (s/f)printf format string vulnerability
    string(CONCAT CMAKE_C_FLAGS " -Wlogical-not-parentheses")  # Unclear boolean expression
    string(CONCAT CMAKE_C_FLAGS " -Wmissing-declarations")  #
    string(CONCAT CMAKE_C_FLAGS " -Wnull-dereference")  # Potential NULL pointer dereference
    string(CONCAT CMAKE_C_FLAGS " -Wduplicated-cond")  # Checking same thing twice

    # NOTE: deactivating goto warnings: they are used only within the test suite, not the library
    #string(CONCAT CMAKE_C_FLAGS "-Wjump-misses-init")  # Switch/goto jump skips variable init

    # Debug mode
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -g3")  # Max debug info
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -O0")  # Do not optimise
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -coverage")  # Gather code coverage info
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -static-libsan")
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -fsanitize=address,undefined")
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -fno-omit-frame-pointer")
    string(CONCAT CMAKE_C_FLAGS_DEBUG " -mno-omit-leaf-frame-pointer")

    # Release mode optimised for speed
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -O3")  # Max optimisation for speed
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -Werror")  # Warnings as errors
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -fomit-frame-pointer")
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -march=native")
    string(CONCAT CMAKE_C_FLAGS_RELEASE " -mtune=native")
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -funroll-loops")

    # Release mode optimised for size
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -Os")  # Optimise for size
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -fomit-frame-pointer")
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -march=native")
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL " -mtune=native")

elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using compiler flags for Microsoft Visual C++ compiler CL (MSVC)")

    # Activate a million warnings to have the cleanest possible code
    string(CONCAT CMAKE_C_FLAGS "-Wall")  # Activate all warnings
    # (compared to GCC, this actually turns on ALL of the warnings, not just most)
    string(CONCAT CMAKE_C_FLAGS "-Qspectre")  # Let the compiler inject Spectre mitigation code

    # Suppress the warning about Spectre mitigation.
    # The problem is that even when the Spectre mitigation flag is enabled, the warning about
    # the mitigation being required still appears, so we have to forcibly disable it.
    string(CONCAT CMAKE_C_FLAGS "-wd5045")

    # Suppress warning about deprecated stdio functions.
    # The warning notifies that the functions should be replaced with the safer C11 alternatives
    # fopen -> fopen_s, fscanf -> fscanf_s etc. Here they are only used in the test framework,
    # not in the Ascon implementation, so they are not critical. They are also used in a safe
    # manner to start with, given that the parsed data is fixed (the test vectors).
    # Finally, not every clib implements them, so we cannot assume the compilation succeeds
    # if we use them. Thus, better deactivated.
    string(CONCAT CMAKE_C_FLAGS "-wd4996")

    # Suppress warning about constant conditional expr.
    # This warning only pops up in the test suite's checks of the Ascon context and state struct
    # sizes, which ARE constant. The tests are there just as a double-check, an assertion,
    # and must stay, so the warning is disabled.
    string(CONCAT CMAKE_C_FLAGS "-wd4127")

    # Suppress informational warning about inlining.
    # MSVC notifies with a warning when the optimised inlines/does not inline a function that
    # is not/is marked for inlining. The warning is informational, thus disabled.
    string(CONCAT CMAKE_C_FLAGS "-wd4710 -wd4711")

    # Debug mode
    string(CONCAT CMAKE_C_FLAGS_DEBUG "-Od")  # Do not optimise

    # Release mode optimised for speed
    string(CONCAT CMAKE_C_FLAGS_RELEASE "-O2")  # Optimise for speed
    string(CONCAT CMAKE_C_FLAGS_RELEASE "-WX")  # Warnings as errors

    # Release mode optimised for size
    string(CONCAT CMAKE_C_FLAGS_MINSIZEREL "-O1")  # Optimise for size
    string(CONCAT CMAKE_C_FLAGS_RELEASE "-WX")  # Warnings as errors
else ()
    message(WARNING "\
Unknown C compiled with ID '${CMAKE_C_COMPILER_ID}' for project ${PROJECT_NAME}.
Not applying any project-specific compilation flag, leaving all to defaults.")
endif ()

# Options for all compilers
string(CONCAT CMAKE_C_FLAGS_DEBUG "-DDEBUG")
string(CONCAT CMAKE_C_FLAGS_RELEASE "-DRELEASE")
string(CONCAT CMAKE_C_FLAGS_MINSIZEREL "-DMINSIZEREL")

# Information for the user
message(STATUS "Used compiler flags: CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "Used compiler flags: CMAKE_C_FLAGS_DEBUG = ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Used compiler flags: CMAKE_C_FLAGS_RELEASE = ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "Used compiler flags: CMAKE_C_FLAGS_MINSIZEREL = ${CMAKE_C_FLAGS_MINSIZEREL}")
