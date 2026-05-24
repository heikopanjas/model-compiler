cmake_minimum_required(VERSION 3.20)
project(model_compiler CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent for external dependencies
include(FetchContent)

# Fetch cxxopts for command line parsing
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.3.1
)

# Disable cxxopts features we don't need
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_WARNINGS OFF CACHE BOOL "" FORCE)
set(CXXOPTS_USE_UNICODE_HELP OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cxxopts)

# Find Flex and Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Flex and Bison sources
set(LEXER_SOURCE ${CMAKE_SOURCE_DIR}/src/model-compiler.l)
set(PARSER_SOURCE ${CMAKE_SOURCE_DIR}/src/model-compiler.y)

# Generate lexer and parser
FLEX_TARGET(Lexer ${LEXER_SOURCE} ${CMAKE_BINARY_DIR}/lexer.cpp)
BISON_TARGET(Parser ${PARSER_SOURCE} ${CMAKE_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_BINARY_DIR}/parser.h)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
)

# --- Runtime platform dependencies --------------------------------------------
# The runtime (include/runtime/) requires platform-specific UUID libraries.
# This INTERFACE target carries the correct link dependencies so that any
# target compiling Guid.cpp can simply link against bbfm_runtime.
add_library(bbfm_runtime INTERFACE)
target_include_directories(bbfm_runtime INTERFACE ${CMAKE_SOURCE_DIR}/include)

if (WIN32)
    # ole32.lib is auto-linked via #pragma comment(lib) in Guid.cpp
elseif (APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    target_link_libraries(bbfm_runtime INTERFACE ${COREFOUNDATION_LIBRARY})
else()
    # Linux - require libuuid (uuid-dev / libuuid-devel)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(UUID REQUIRED uuid)
    target_include_directories(bbfm_runtime INTERFACE ${UUID_INCLUDE_DIRS})
    target_link_libraries(bbfm_runtime INTERFACE ${UUID_LIBRARIES})
endif()

# Compiler executable
add_executable(model-compiler
    src/main.cpp
    src/Driver.cpp
    src/AST.cpp
    src/SemanticAnalyzer.cpp
    src/Console.cpp
    src/CodeGenerator.cpp
    src/CppCodeGenerator.cpp
    src/RustCodeGenerator.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# Compiler flags - different for MSVC vs GCC/Clang
if (MSVC)
    target_compile_options(model-compiler PRIVATE
        /W4
        # Suppress warnings from flex/bison generated code
        /wd4005  # Macro redefinition (stdint.h INT*_MIN/MAX in flex output)
        /wd4702  # Unreachable code in bison-generated parser
    )
else()
    target_compile_options(model-compiler PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Link cxxopts as static library
target_link_libraries(model-compiler PRIVATE cxxopts::cxxopts)

# Link with necessary libraries (if flex/bison need them)
if (APPLE)
    target_link_libraries(model-compiler)
endif()

# Install target
install(TARGETS model-compiler DESTINATION bin)

# --- Tests --------------------------------------------------------------------
enable_testing()

# Helper to register a test that must exit 0
function(add_fm_test name file)
    add_test(NAME ${name}
             COMMAND model-compiler ${CMAKE_SOURCE_DIR}/examples/${file})
endfunction()

# Helper to register a test that must exit non-zero (error expected)
function(add_fm_error_test name file)
    add_test(NAME ${name}
             COMMAND model-compiler ${CMAKE_SOURCE_DIR}/examples/${file})
    set_tests_properties(${name} PROPERTIES WILL_FAIL TRUE)
endfunction()

# Positive tests -- compiler must succeed (exit 0)
add_fm_test(podcast                       podcast.fm)
add_fm_test(comprehensive                 comprehensive_test.fm)
add_fm_test(computed_simple               test_computed_simple.fm)
add_fm_test(computed_member_access        test_computed_member_access.fm)
add_fm_test(computed_inheritance          test_computed_inheritance.fm)
add_fm_test(expressions                   test_expressions.fm)
add_fm_test(namespace                     test_namespace.fm)
add_fm_test(type_promotion_ok             test_type_promotion_ok.fm)
# error_test_suite.fm is a documentation/index file (all cases are block-commented);
# it compiles to an empty input so it belongs with the positive tests.
add_fm_test(error_suite_doc               error_test_suite.fm)

# Negative tests -- compiler must detect the error (exit non-zero)
add_fm_error_test(error_bad_invariant              test_bad_invariant.fm)
add_fm_error_test(error_circular_inheritance       test_circular_inheritance.fm)
add_fm_error_test(error_computed_invalid_member    test_computed_error_invalid_member.fm)
add_fm_error_test(error_computed_member_primitive  test_computed_error_member_on_primitive.fm)
add_fm_error_test(error_computed_undefined_field   test_computed_error_undefined_field.fm)
add_fm_error_test(error_computed_optional          test_computed_optional_error.fm)
add_fm_error_test(error_duplicate_field            test_duplicate_field.fm)
add_fm_error_test(error_column_reporting           test_error_column.fm)
add_fm_error_test(error_missing_semicolon          test_missing_semicolon.fm)
add_fm_error_test(error_type_int_real              test_type_error_int_real.fm)
add_fm_error_test(error_type_member_access         test_type_error_member_access.fm)
add_fm_error_test(error_undefined_type             test_undefined_type.fm)
add_fm_error_test(error_wrong_case                 test_wrong_case.fm)

# --- Runtime unit tests -------------------------------------------------------

add_executable(guid-test
    tests/runtime/GuidTest.cpp
    include/runtime/Guid.cpp
)

target_link_libraries(guid-test PRIVATE bbfm_runtime)

if (MSVC)
    target_compile_options(guid-test PRIVATE /W4)
else()
    target_compile_options(guid-test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME runtime_guid COMMAND guid-test)
