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

# Compiler executable
add_executable(model-compiler
    src/main.cpp
    src/Driver.cpp
    src/AST.cpp
    src/SemanticAnalyzer.cpp
    src/Console.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# Compiler flags
target_compile_options(model-compiler PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Link cxxopts as static library
target_link_libraries(model-compiler PRIVATE cxxopts::cxxopts)

# Link with necessary libraries (if flex/bison need them)
if (APPLE)
    target_link_libraries(model-compiler)
endif()

# Install target
install(TARGETS model-compiler DESTINATION bin)
