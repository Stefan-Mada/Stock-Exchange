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

TEST_CASE("Simplest Execution") {
    OrderBook orderBook;

    auto buyOrder = orderBook.addOrder(buy, 5, 2);
    auto sellOrder = orderBook.addOrder(sell, 5, 2);
    CHECK(buyOrder.getBaseId() + 1 == sellOrder.getBaseId());

    CHECK(orderBook.getTotalVolume() == 5);
    CHECK(orderBook.getVolumeAtLimit(2) == 5);
    CHECK(sellOrder.getMoneyExchanged() == 10);
    CHECK(sellOrder.getTotalSharesExecuted() == 5);
}

TEST_CASE("Double Execution with no partial execution") {
    OrderBook orderBook;
    auto buyOrder = orderBook.addOrder(buy, 10, 4);
    auto sellOrder1 = orderBook.addOrder(sell, 5, 4);
    auto sellOrder2 = orderBook.addOrder(sell, 5, 4);

    CHECK(buyOrder.getFulfilledOrderIds().empty());

    CHECK(buyOrder.getBaseId() + 2 == sellOrder2.getBaseId());

    CHECK(orderBook.getTotalVolume() == 10);
    CHECK(orderBook.getVolumeAtLimit(4) == 10);
    CHECK(orderBook.getVolumeAtLimit(5) == 0);
    CHECK(sellOrder1.getMoneyExchanged() == 20);
    CHECK(sellOrder2.getTotalSharesExecuted() == 5);

    CHECK(!sellOrder1.hasPartialExecution());
    CHECK(!sellOrder2.hasPartialExecution());
}

TEST_CASE("Test empty best bid/asks") {
    OrderBook orderBook;

    CHECK(!orderBook.getBestAsk().has_value());
    CHECK(!orderBook.getBestBid().has_value());

    auto buyOrder = orderBook.addOrder(buy, 5, 2);
    auto sellOrder = orderBook.addOrder(sell, 5, 5);

    CHECK(orderBook.getBestAsk().value() == 5);
    CHECK(orderBook.getBestBid().value() == 2);
}

TEST_CASE("Test order cancellation basic") {
    OrderBook orderBook;

    auto buyOrder = orderBook.addOrder(buy, 100, 400);
    int orderId = buyOrder.getBaseId();

    CHECK(orderBook.getBestBid().value() == 400);
    orderBook.cancelOrder(orderId);
    CHECK(!orderBook.getBestBid().has_value());

    auto sellOrder = orderBook.addOrder(sell, 100, 405);
    buyOrder = orderBook.addOrder(buy, 20, 410);

    CHECK(orderBook.getTotalVolume() == 20);
    CHECK(orderBook.getVolumeAtLimit(405) == 20);
    CHECK(buyOrder.getMoneyExchanged() == 8100);
    CHECK(buyOrder.getTotalSharesExecuted() == 20);

    CHECK(orderBook.getBestAsk().value() == 405);
    orderBook.cancelOrder(sellOrder.getBaseId());
    buyOrder = orderBook.addOrder(buy, 20, 410);
    CHECK(!orderBook.getBestAsk().has_value());

    CHECK(buyOrder.getTotalSharesExecuted() == 0);
    CHECK(orderBook.getTotalVolume() == 20);
}

TEST_CASE("One order eats up two and then some") {
    OrderBook orderBook;

    auto sellOrder1 = orderBook.addOrder(sell, 20, 10);
    auto sellOrder2 = orderBook.addOrder(sell, 30, 10);
    // 50 shares selling at 10

    CHECK(orderBook.getBestAsk().value() == 10);
    auto buyOrder = orderBook.addOrder(buy, 60, 11);
    CHECK(!orderBook.getBestAsk().has_value());
    // 10 shares buying at 11

    CHECK(orderBook.getTotalVolume() == 50);
    CHECK(orderBook.getVolumeAtLimit(10) == 50);
    CHECK(buyOrder.getMoneyExchanged() == 500);
    CHECK(buyOrder.getTotalSharesExecuted() == 50);
    CHECK(orderBook.getBestBid().value() == 11);

    auto sellOrder3 = orderBook.addOrder(sell, 10, 11);
    // Should have no shares left

    CHECK(orderBook.getTotalVolume() == 60);
    CHECK(orderBook.getVolumeAtLimit(10) == 50);
    CHECK(orderBook.getVolumeAtLimit(11) == 10);
    CHECK(sellOrder3.getMoneyExchanged() == 110);
    CHECK(sellOrder3.getTotalSharesExecuted() == 10);
    CHECK(!orderBook.getBestBid().has_value());
    CHECK(!orderBook.getBestAsk().has_value());
}


TEST_SUITE_END();