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

# --- Compiler runtime ----------------------------------------------------------
# The runtime library contains reusable compiler/runtime types used by generated
# code, plugin capability exchange, and runtime tests.
add_library(compiler-runtime SHARED
    src/runtime/AST.cpp
    src/TypeSymbol.cpp
    src/runtime/SemanticAnalyzer.cpp
    src/Console.cpp
    src/runtime/ICodeGenerator.cpp
    src/runtime/Fabric.cpp
    src/runtime/Guid.cpp
    src/runtime/String.cpp
    src/runtime/Date.cpp
)

target_include_directories(compiler-runtime PUBLIC ${CMAKE_SOURCE_DIR}/include)

set_target_properties(compiler-runtime PROPERTIES
    PREFIX ""
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

if (MSVC)
    target_compile_options(compiler-runtime PRIVATE /W4)
else()
    target_compile_options(compiler-runtime PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    # ole32.lib is auto-linked via #pragma comment(lib) in Guid.cpp
elseif (APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    target_link_libraries(compiler-runtime PUBLIC ${COREFOUNDATION_LIBRARY})
else()
    # Linux - require libuuid (uuid-dev / libuuid-devel)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(UUID REQUIRED uuid)
    target_include_directories(compiler-runtime PUBLIC ${UUID_INCLUDE_DIRS})
    target_link_libraries(compiler-runtime PUBLIC ${UUID_LIBRARIES})
endif()

# Compiler executable
add_executable(model-compiler
    src/main.cpp
    src/Driver.cpp
    src/SharedLibrary.cpp
    src/GeneratorInstance.cpp
    src/GeneratorPluginManager.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

set_target_properties(model-compiler PROPERTIES ENABLE_EXPORTS ON)

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
target_link_libraries(model-compiler PRIVATE compiler-runtime cxxopts::cxxopts ${CMAKE_DL_LIBS})

# Link with necessary libraries (if flex/bison need them)
if (APPLE)
    target_link_libraries(model-compiler)
endif()

# Generator shared libraries. These are placed directly beside model-compiler
# in the build tree so development and CTest can load them without extra setup.
function(add_generator_plugin target source)
    add_library(${target} SHARED ${source})
    get_filename_component(generator_source_dir ${source} DIRECTORY)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/${generator_source_dir})
    target_link_libraries(${target} PRIVATE compiler-runtime model-compiler)
    set_target_properties(${target} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if (APPLE)
        target_link_options(${target} PRIVATE -Wl,-undefined,dynamic_lookup)
    endif()
endfunction()

add_generator_plugin(c++-generator src/generators/c++/CppCodeGenerator.cpp)
add_generator_plugin(rust-generator src/generators/rust/RustCodeGenerator.cpp)
add_generator_plugin(swift-generator src/generators/swift/SwiftCodeGenerator.cpp)

add_library(not-a-plugin SHARED tests/generator/NotAPlugin.cpp)
set_target_properties(not-a-plugin PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Install target
install(TARGETS compiler-runtime model-compiler c++-generator rust-generator swift-generator DESTINATION bin)

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

# Code generation tests -- compiler must load generator plugins from the
# executable directory and write target-language output.
function(add_codegen_test name language file output)
    add_test(NAME ${name}
             COMMAND model-compiler
                     --lang ${language}
                     --plugin-dir $<TARGET_FILE_DIR:model-compiler>
                     -o ${CMAKE_BINARY_DIR}/${output}
                     ${CMAKE_SOURCE_DIR}/examples/${file})
endfunction()

add_codegen_test(codegen_cpp_plugin      c++    test_computed_simple.fm generated_cpp_plugin.h)
add_codegen_test(codegen_rust_plugin     rust   test_computed_simple.fm generated_rust_plugin.rs)
add_codegen_test(codegen_swift_plugin    swift  test_computed_simple.fm generated_swift_plugin.swift)

add_test(NAME list_languages
         COMMAND model-compiler
                 --list-languages
                 --plugin-dir $<TARGET_FILE_DIR:model-compiler>)

add_test(NAME codegen_missing_language
         COMMAND model-compiler
                 --lang zig
                 --plugin-dir $<TARGET_FILE_DIR:model-compiler>
                 -o ${CMAKE_BINARY_DIR}/generated_missing_language.txt
                 ${CMAKE_SOURCE_DIR}/examples/test_computed_simple.fm)
set_tests_properties(codegen_missing_language PROPERTIES WILL_FAIL TRUE)

# --- Runtime unit tests -------------------------------------------------------

add_executable(guid-test
    tests/runtime/GuidTest.cpp
)

target_link_libraries(guid-test PRIVATE compiler-runtime)

add_executable(string-test
    tests/runtime/StringTest.cpp
)

target_link_libraries(string-test PRIVATE compiler-runtime)

add_executable(dictionary-test
    tests/runtime/DictionaryTest.cpp
)

target_link_libraries(dictionary-test PRIVATE compiler-runtime)

add_executable(array-test
    tests/runtime/ArrayTest.cpp
)

target_link_libraries(array-test PRIVATE compiler-runtime)

if (MSVC)
    target_compile_options(guid-test PRIVATE /W4)
    target_compile_options(string-test PRIVATE /W4)
    target_compile_options(dictionary-test PRIVATE /W4)
    target_compile_options(array-test PRIVATE /W4)
else()
    target_compile_options(guid-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(string-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(dictionary-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(array-test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME runtime_guid COMMAND guid-test)
add_test(NAME runtime_string COMMAND string-test)
add_test(NAME runtime_dictionary COMMAND dictionary-test)
add_test(NAME runtime_array COMMAND array-test)
