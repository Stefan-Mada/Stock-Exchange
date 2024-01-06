cmake_minimum_required(VERSION 3.22)
project(Stock-Exchange VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

# Set list of all project headers
set(STOCKEXCHANGE_SRCS  src/stockExchange.cpp
                        src/orderBook.cpp)

# Create actual executable
# add_executable(Stock-Exchange ${STOCKEXCHANGE_SRCS})

# Set include directory
# target_include_directories(Stock-Exchange PRIVATE include)

# Set up testing framework
set(TEST_SOURCES tests/main.cpp tests/orderBook.test.cpp)

# Make tests executable
add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE include)
target_compile_features(tests PRIVATE cxx_std_20)

# enable testing functionality
enable_testing()

#define the tests
add_test(
    NAME tests
    COMMAND $<TARGET_FILE:tests>
)

# run tests after build
add_custom_command(
    TARGET tests
    POST_BUILD
    COMMENT "Run tests after build"
    COMMAND ctest --output-on-failure
)