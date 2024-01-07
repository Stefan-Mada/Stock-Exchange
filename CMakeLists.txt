# Wonderful CMake file!
# Builds stock-exchange, tests, and coverage by default
# Build target "debug-tests" in order to run tests with and without sanitizers on
# Strictly speaking, not cross platform, but only with regards to testing

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
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping -fsanitize=address,undefined,leak -fno-sanitize-recover=address,undefined,leak ${CMAKE_CXX_FLAGS_DEBUG}")
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

# Ensure we have the proper program to run coverage tests
find_program(LLVM_COV llvm-cov llvm-cov-14 REQUIRED)
find_program(LLVM_PROFDATA llvm-profdata llvm-profdata-14 REQUIRED)
find_program(GENHTML genhtml REQUIRED)

# Run boatloads of commands to generate LLVM traces, reports, and then use genHTML to get a prettier version
# End up with 2 htmls to look over code coverage with
# Second command is a mystical one to fix issue with "cannot merge previous GCDA file"
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_custom_target(coverage ALL 
        COMMAND ${CMAKE_COMMAND} -E rm -rf -- default.profraw tests.profdata ${CMAKE_SOURCE_DIR}/default.profraw
        COMMAND find . -name "*.gcda" -print0 | xargs -0 rm || true
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/coverage
        COMMAND ${LLVM_PROFDATA} merge -sparse default.profraw -o tests.profdata
        COMMAND ${LLVM_COV} show ./tests -instr-profile=tests.profdata ${CMAKE_SOURCE_DIR}/src/ -show-regions --show-branches=count --show-expansions -format=html > coverage/LLVM-index.html
        COMMAND ${LLVM_COV} export ./tests -instr-profile=tests.profdata ${CMAKE_SOURCE_DIR}/src/ -format=lcov > coverage/coverage.lcov
        COMMAND ${GENHTML} --demangle-cpp -o coverage coverage/coverage.lcov
        COMMAND ${LLVM_COV} report ./tests -instr-profile=tests.profdata ${CMAKE_SOURCE_DIR}/src/
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Make sure tests build before coverage runs
    add_dependencies(coverage tests)
else()
    # Doesn't actually run coverage in release mode, just tests
    add_custom_command(
        TARGET tests 
        POST_BUILD
        COMMAND ctest --output-on-failure
    )
endif()

