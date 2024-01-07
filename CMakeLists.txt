# Wonderful CMake file!
# Builds stock-exchange, tests, and coverage by default
# Build target "debug-tests" in order to run tests with and without sanitizers on

cmake_minimum_required(VERSION 3.22)
project(Stock-Exchange VERSION 1.0 LANGUAGES CXX)

# Set default build type to debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wshadow")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${CMAKE_CXX_FLAGS_RELEASE}")


# Set list of all project sources, excluding main file
set(STOCKEXCHANGE_SRCS  src/orderBook.cpp
                        src/limitPrice.cpp
                        src/order.cpp
                        src/orderExecution.cpp)

# Turn sources into a static library for use in testing AND in main executable
add_library(StockExchangeLib STATIC ${STOCKEXCHANGE_SRCS})

# Create actual executable
add_executable(Stock-Exchange src/stockExchange.cpp)

# Set include directory for main executable
target_include_directories(Stock-Exchange PRIVATE include)

# Link executable against library
target_link_libraries(Stock-Exchange PRIVATE StockExchangeLib)

# Have executable force C++20
target_compile_features(Stock-Exchange PRIVATE cxx_std_20)



# ============ TESTING ===============

# Set list of all test sources, INCLUDING test main
set(TEST_SOURCES tests/main.cpp tests/orderBook.test.cpp)

# Make tests executable
add_executable(tests ${TEST_SOURCES})

# Do all linking business, same as above really
target_include_directories(tests PRIVATE include src)
target_link_libraries(tests PRIVATE StockExchangeLib)
target_compile_features(tests PRIVATE cxx_std_20)

# enable testing functionality
enable_testing()

#define the tests
add_test(
    NAME tests
    COMMAND $<TARGET_FILE:tests>
)

# custom target to run coverage tests
# KCov doesn't support sanitizers (of course)
# So testing should be run twice: once with sanitizers, and once without
add_custom_target(coverage ALL 
    COMMAND kcov --include-path=${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/coverage ${CMAKE_CURRENT_BINARY_DIR}/tests
)

# Make sure tests build before coverage runs
add_dependencies(coverage tests)

# =========== DEBUG TESTING ==========
# Debug test target runs coverage tests AND builds with sanitizers, running
# unit tests again. This is because the code coverage can't run with sanitizers,
# but this allows full testing of the code

# New executable to re-compile with sanitizers
# Must add sources again to recompile with sanitizers
add_executable(sanitizer-tests EXCLUDE_FROM_ALL ${TEST_SOURCES} ${STOCKEXCHANGE_SRCS})

# Do all linking business, same as above really
target_include_directories(sanitizer-tests PRIVATE include src)
target_compile_features(sanitizer-tests PRIVATE cxx_std_20)

# Make sure this new target runs with additional NEW compiler options
# Need to use target_link_libraries here for the sanitizers
target_compile_options(sanitizer-tests PRIVATE -fsanitize=address,undefined,leak -fno-sanitize-recover=address,undefined,leak)
target_link_libraries(sanitizer-tests -fsanitize=address,undefined,leak)

# Run new tests
add_custom_target(debug-tests 
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/sanitizer-tests
)

# Make this new target depend on code coverage FIRST
add_dependencies(debug-tests coverage sanitizer-tests)
