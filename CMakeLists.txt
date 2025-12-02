cmake_minimum_required(VERSION 3.16)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
        "In-source builds are not allowed. "
        "Please create a separate build directory and run cmake from there:\n"
        "  mkdir build && cd build && cmake .."
    )
endif()

project(router
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "IPv4 Router Simulator for CSCI471"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use ccache if available for faster rebuilds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release"
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Detect compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(COMPILER_GCC TRUE)
  message(STATUS "Compiler: GCC ${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(COMPILER_CLANG TRUE)
  message(STATUS "Compiler: Clang ${CMAKE_CXX_COMPILER_VERSION}")
else()
  message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

set(WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Werror=return-type
)

if(COMPILER_GCC)
  list(APPEND WARNING_FLAGS
        -Wmisleading-indentation    # Warn about misleading indentation
        -Wduplicated-cond           # Warn about duplicated conditions
        -Wduplicated-branches       # Warn about duplicated branches
        -Wlogical-op                # Warn about logical operation issues
        -Wuseless-cast              # Warn about useless casts
    )
endif()

# Clang-specific warnings
if(COMPILER_CLANG)
  list(APPEND WARNING_FLAGS
        -Wmost                      # Most warnings
        -Wthread-safety             # Thread safety annotations
    )
endif()

# ============================================================================
# Build Type Specific Flags
# ============================================================================

set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

set(SANITIZER_FLAGS "")

if(ENABLE_ASAN)
  message(STATUS "AddressSanitizer enabled")
  list(APPEND SANITIZER_FLAGS -fsanitize=address -fno-omit-frame-pointer)
endif()

if(ENABLE_UBSAN)
  message(STATUS "UndefinedBehaviorSanitizer enabled")
  list(APPEND SANITIZER_FLAGS -fsanitize=undefined)
endif()

if(ENABLE_TSAN)
  if(ENABLE_ASAN)
    message(FATAL_ERROR "ThreadSanitizer cannot be used with AddressSanitizer")
  endif()
  message(STATUS "ThreadSanitizer enabled")
  list(APPEND SANITIZER_FLAGS -fsanitize=thread)
endif()

option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck static analysis" OFF)

if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_PROGRAM clang-tidy)
  if(CLANG_TIDY_PROGRAM)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_PROGRAM}")
    set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY_PROGRAM};
            -checks=*,-fuchsia-*,-llvmlibc-*,-altera-*;
            -warnings-as-errors=*;
        )
  else()
    message(WARNING "clang-tidy requested but not found")
  endif()
endif()

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK_PROGRAM cppcheck)
  if(CPPCHECK_PROGRAM)
    message(STATUS "cppcheck found: ${CPPCHECK_PROGRAM}")
    set(CMAKE_CXX_CPPCHECK
            ${CPPCHECK_PROGRAM};
            --enable=all;
            --suppress=missingIncludeSystem;
            --inline-suppr;
            --inconclusive;
        )
  else()
    message(WARNING "cppcheck requested but not found")
  endif()
endif()

set(ROUTER_SOURCES
    src/main.cpp
    src/IPv4Header.cpp
)

set(ROUTER_HEADERS
    src/IPv4Header.hpp
)

add_executable(router ${ROUTER_SOURCES} ${ROUTER_HEADERS})

target_compile_options(router PRIVATE ${WARNING_FLAGS})

if(SANITIZER_FLAGS)
  target_compile_options(router PRIVATE ${SANITIZER_FLAGS})
  target_link_options(router PRIVATE ${SANITIZER_FLAGS})
endif()

target_include_directories(router
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(router PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)

find_program(CLANG_FORMAT_PROGRAM clang-format)
if(CLANG_FORMAT_PROGRAM)
  add_custom_target(format
        COMMAND ${CLANG_FORMAT_PROGRAM} -i ${ROUTER_SOURCES} ${ROUTER_HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files with clang-format"
        VERBATIM
    )
endif()

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/router
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Cleaning all build artifacts"
)

message(STATUS "")
message(STATUS "\tConfiguration Summary")
message(STATUS "\tProject:\t\t${PROJECT_NAME}\tv${PROJECT_VERSION}")
message(STATUS "\tBuild type:\t\t${CMAKE_BUILD_TYPE}")
message(STATUS "\tC++ Standard:\t\t${CMAKE_CXX_STANDARD}")
message(STATUS "\tCompiler:\t\t${CMAKE_CXX_COMPILER_ID}\t${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "\tInstall prefix:\t\t${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Options:")
message(STATUS "\tENABLE_ASAN:\t\t${ENABLE_ASAN}")
message(STATUS "\tENABLE_UBSAN:\t\t${ENABLE_UBSAN}")
message(STATUS "\tENABLE_TSAN:\t\t${ENABLE_TSAN}")
message(STATUS "\tENABLE_CLANG_TIDY:\t\t${ENABLE_CLANG_TIDY}")
message(STATUS "\tENABLE_CPPCHECK:\t\t${ENABLE_CPPCHECK}")
message(STATUS "")
