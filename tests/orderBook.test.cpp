/**
 * @file orderBook.test.cpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Unit tests for orderbook class
 * @version 1.0
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "doctest.h"
#include "orderBook.hpp"

using namespace Exchange;
using enum OrderType;

TEST_SUITE_BEGIN("orderBook");

TEST_CASE("Simple Execution") {
    OrderBook orderBook;

    auto buyOrder = orderBook.addOrder(buy, 5, 2);
    auto sellOrder = orderBook.addOrder(sell, 5, 2);
    CHECK(buyOrder.getBaseId() + 1 == sellOrder.getBaseId());

    CHECK(orderBook.getTotalVolume() == 5);
    CHECK(orderBook.getVolumeAtLimit(2) == 5);
    CHECK(sellOrder.getMoneyExchanged() == 10);
    CHECK(sellOrder.getTotalSharesExecuted() == 5);
}

TEST_CASE("Test empty best bid/asks") {

}

TEST_SUITE_END();