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
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address,undefined -fno-sanitize-recover=address,undefined ${CMAKE_CXX_FLAGS_DEBUG}")
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

# run tests after build
# add_custom_command(
#     TARGET tests
#     POST_BUILD
#     COMMENT "Run tests after build"
#     COMMAND ctest --output-on-failure
# )