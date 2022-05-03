# CMake configuration to build Xtr
# License: BSD 3-Clause
#
# For example usage: see README.md

cmake_minimum_required(VERSION 3.9)
project(Xtr
        VERSION 0.1.0
        LANGUAGES C
        DESCRIPTION
        "Extendible strings for C")

# Default build type, if not specified explicitly with `-DCMAKE_BUILD_TYPE`
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
    message("CMAKE_BUILD_TYPE unspecified, defaulting to ${CMAKE_BUILD_TYPE}")
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# Compiler flags
# -----------------------------------------------------------------------------
include(compiler_flags.cmake)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("assert.h" ASSERT_H_EXISTS)
if (ASSERT_H_EXISTS)
    message(STATUS "assert.h found. Enabling runtime assertions verifying "
            "the library API inputs using assert() but only in debug mode "
            "for all targets.")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -DXTR_INPUT_ASSERTS")
endif ()


# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
set(LIB_HEADER src/xtr.h)
set(LIB_SRC ${LIB_HEADER} src/xtr.c)


# -----------------------------------------------------------------------------
# Build targets
# -----------------------------------------------------------------------------
# Copy the library API header into the build directory.
# With this command, if the header changes, it's copied, otherwise not.
configure_file(${LIB_HEADER} ${CMAKE_BINARY_DIR} COPYONLY)

# Static libraries (.a), compiled with all features or only some features
add_library(xtr_static STATIC ${LIB_SRC})
target_include_directories(xtr_static PUBLIC inc/ PRIVATE src/)
set_target_properties(xtr_static PROPERTIES
        INTERPROCEDURAL_OPTIMISATION TRUE)

# Shared library (.so / .dylib / .dll)
# Does not reuse the static library object files, as they are
# recompiled in order to have position-independent code
add_library(xtr SHARED ${LIB_SRC})
target_include_directories(xtr PUBLIC inc/ PRIVATE src/)
set_target_properties(xtr PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        INTERPROCEDURAL_OPTIMISATION TRUE
        # Remove any "msys-" and enforce the same lib name with all toolchains
        PREFIX lib
        OUTPUT_NAME xtr)

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
    set(DOXYGEN_IMAGE_PATH ${PROJECT_SOURCE_DIR}/images)

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
        set(DOXYGEN_DOT_PATH ) # Empty = find it in PATH
        set(DOXYGEN_DOT_NUM_THREADS 0)  # As many as CPU cores
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_DIRECTORY_GRAPH YES)
    endif()

        # Customisations
    set(DOXYGEN_ALIASES license="**License:**")
    set(DOXYGEN_PREDEFINED WIN32 ASCON_INPUT_ASSERTS)

    doxygen_add_docs(ascon_doxygen
            # Do NOT build doxygen on make-all, to avoid polluting the stdout
            # List of input files for Doxygen
            ${PROJECT_SOURCE_DIR}/inc/ascon.h
            ${PROJECT_SOURCE_DIR}/LICENSE.md
            ${PROJECT_SOURCE_DIR}/AUTHORS.md
            ${PROJECT_SOURCE_DIR}/README.md
            ${PROJECT_SOURCE_DIR}/CHANGELOG.md
            COMMENT "Generating Doxygen documentation...")
else (DOXYGEN_FOUND)
    message(WARNING "Doxygen not found. Cannot generate documentation.")
endif (DOXYGEN_FOUND)

enable_testing()
add_test(NAME testascon COMMAND ./testascon)
add_test(NAME testasconshared COMMAND ./testasconshared)
