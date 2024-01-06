cmake_minimum_required(VERSION 3.22)
project(Stock-Exchange VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

# Set list of all project headers
set(STOCKEXCHANGE_HEADERS src/limitPrice.hpp src/order.hpp src/orderBook.hpp src/orderExecution.hpp)

# Create actual executable
add_executable(Stock-Exchange   src/stockExchange.cpp
                                ${STOCKEXCHANGE_HEADERS})

# Set include directory
target_include_directories(Stock-Exchange PRIVATE include)