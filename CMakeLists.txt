cmake_minimum_required(VERSION 3.20)
project(p3_compiler C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Flex and Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Flex and Bison sources
set(LEXER_SOURCE ${CMAKE_SOURCE_DIR}/src/lexer.l)
set(PARSER_SOURCE ${CMAKE_SOURCE_DIR}/src/parser.y)

# Generate lexer and parser
FLEX_TARGET(Lexer ${LEXER_SOURCE} ${CMAKE_BINARY_DIR}/lexer.c)
BISON_TARGET(Parser ${PARSER_SOURCE} ${CMAKE_BINARY_DIR}/parser.c
    DEFINES_FILE ${CMAKE_BINARY_DIR}/parser.h)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
)

# Compiler executable
add_executable(p3c
    src/main.c
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# Compiler flags
target_compile_options(p3c PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Install target
install(TARGETS p3c DESTINATION bin)
