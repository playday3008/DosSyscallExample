cmake_minimum_required(VERSION 3.21)
project(
    DosSyscallExample
    LANGUAGES C CXX
    VERSION 0.1.0
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# OpenWatcom-specific compiler options
if(CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    add_compile_options(
        -zq         # Operate quietly (display only error messages)
        -wx         # Maximum warning level
        -fr=nul     # Set error file to NUL (discard error file output)
    )

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "I86")
        # 16-bit DOS
        add_compile_options(
            -bt=dos     # Build target is DOS
            -ms         # Small memory model (small code/small data)
            -0          # 8086 instructions
        )
    else()
        # 32-bit DOS (dos4gw extender)
        add_compile_options(
            -bt=dos4g   # Build target is DOS/4GW protected mode
            -5s         # Pentium stack calling conventions
            -s          # Remove stack overflow checks
        )
    endif()

    string(JOIN " " CMAKE_C_FLAGS
        ${CMAKE_C_FLAGS}
        -za99       # Disable extensions, use C99 standard
    )
    string(JOIN " " CMAKE_CXX_FLAGS
        ${CMAKE_CXX_FLAGS}
        -ze         # Enable extensions (near, far, export, etc.)
    )
endif()

# Add source files and create executable target
add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/dossyscall.c
)

# Include directories for the project
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Set the output name of the executable to short name to avoid issues with long filenames in DOS
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "dossyscl")

# Open Watcom built-in predefined macros — the compiler defines these internally but
# does not expose them to CMake, so IntelliSense (via CMake Tools configurationProvider)
# cannot discover them. We query the compiler with -pm (print macros) using the same
# target flags, parse the output, and re-inject them as compile definitions. This is a
# harmless no-op for the real compiler (same-value redefinition) and makes the macros
# visible to the CMake File API for IntelliSense.
if(CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    # Collect all compile options and pass them to -pm so the macro dump
    # reflects the actual build flags. Generator expressions like
    # $<$<CONFIG:Debug>:-d2> are resolved for the active CMAKE_BUILD_TYPE
    # since execute_process runs at configure time, not build time.
    get_directory_property(_WC_ALL_OPTIONS COMPILE_OPTIONS)
    set(_WC_PM_FLAGS -pm)
    foreach(_opt IN LISTS _WC_ALL_OPTIONS)
        if(_opt MATCHES "^\\$<\\$<CONFIG:${CMAKE_BUILD_TYPE}>:(.+)>$")
            list(APPEND _WC_PM_FLAGS "${CMAKE_MATCH_1}")
        elseif(NOT _opt MATCHES "^\\$<")
            list(APPEND _WC_PM_FLAGS "${_opt}")
        endif()
    endforeach()

    execute_process(
        COMMAND ${CMAKE_C_COMPILER} ${_WC_PM_FLAGS} /dev/null
        OUTPUT_VARIABLE _WC_PM_OUTPUT
        RESULT_VARIABLE _WC_PM_RESULT
        ERROR_VARIABLE _WC_PM_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(VERBOSE "[Watcom -pm] Command: ${CMAKE_C_COMPILER} ${_WC_PM_FLAGS} /dev/null")
    message(VERBOSE "[Watcom -pm] Exit code: ${_WC_PM_RESULT}")
    if(_WC_PM_ERROR)
        message(VERBOSE "[Watcom -pm] Stderr: ${_WC_PM_ERROR}")
    endif()
    message(VERBOSE "[Watcom -pm] Output:\n${_WC_PM_OUTPUT}")

    # Parse "#define NAME VALUE" lines into NAME=VALUE compile definitions
    string(REGEX MATCHALL "#define [^\n]+" _WC_MACRO_LINES "${_WC_PM_OUTPUT}")
    set(_WC_DEFINES "")
    foreach(_line IN LISTS _WC_MACRO_LINES)
        if(_line MATCHES "^#define ([^ ]+) (.+)$")
            list(APPEND _WC_DEFINES "${CMAKE_MATCH_1}=${CMAKE_MATCH_2}")
        elseif(_line MATCHES "^#define ([^ ]+)$")
            list(APPEND _WC_DEFINES "${CMAKE_MATCH_1}")
        endif()
    endforeach()

    target_compile_definitions(${PROJECT_NAME} PRIVATE ${_WC_DEFINES})
endif()
