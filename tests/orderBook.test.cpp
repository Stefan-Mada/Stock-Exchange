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

#include <stdexcept>
#include <utility>
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

    CHECK(sellOrder1.hasPartialExecution());
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

TEST_CASE("Orders executing past one another, check volume after limit disappears") {
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

    // Try re-instating an old limit
    auto buyOrder2 = orderBook.addOrder(buy, 15, 11);
    CHECK(orderBook.getVolumeAtLimit(11) == 10);
    auto sellOrder4 = orderBook.addOrder(sell, 20, 11);
    CHECK(orderBook.getVolumeAtLimit(11) == 25);
    CHECK(orderBook.getBestAsk().value() == 11);
    // Should have 5 selling at 11

    auto buyOrder3 = orderBook.addOrder(buy, 5, 11);
    CHECK(!orderBook.getBestBid().has_value());
    CHECK(!orderBook.getBestAsk().has_value());
}

TEST_CASE("LimitPrice exceptions") {
    LimitPrice limitPrice{10};
    limitPrice.addOrder(Order{0, buy, 10, 10});

    CHECK_THROWS_AS(limitPrice.executeNumberOfShares(1, 20), std::invalid_argument);
}

TEST_CASE("Check executed order info correct") {
    OrderBook orderBook;
    auto sellOrder1 = orderBook.addOrder(sell, 20, 10);
    auto sellOrder2 = orderBook.addOrder(sell, 30, 10);

    const int baseId = sellOrder1.getBaseId();

    auto buyOrder = orderBook.addOrder(buy, 45, 11);

    CHECK(buyOrder.getFulfilledOrderIds().size() == 1);
    CHECK(buyOrder.hasPartialExecution());

    CHECK(buyOrder.getPartiallyFulfilledOrder().value() == std::make_pair(baseId + 1, 25));
    CHECK(buyOrder.getTotalSharesExecuted() == 45);
    CHECK(buyOrder.getMoneyExchanged() == 450);
}

TEST_CASE("Check for time in force") {
    Order order{0, sell, 5, 5, 30};
    CHECK(order.getTimeInForce() == 30);
}

TEST_CASE("OrderExecution throwing for invalid addition") {
    OrderBook orderBook;
    auto sellOrder1 = orderBook.addOrder(sell, 20, 10);
    auto sellOrder2 = orderBook.addOrder(sell, 30, 10);

    auto buyOrder1 = orderBook.addOrder(buy, 20, 10);
    auto buyOrder2 = orderBook.addOrder(buy, 20, 10);

    CHECK_THROWS_AS(buyOrder1 += buyOrder2, std::invalid_argument);

    // Other throw condition requires really trying to do something bad
    // Practically impossible unless I code something wrong in the logic
    OrderExecution exec1{5};
    OrderExecution exec2{5};

    Order order1{6, buy, 7, 10};
    exec1.executeOrder(order1, 5);
    exec2.executeOrder(order1, 5);
    // Now both executions have a partial execution
    CHECK_THROWS_AS(exec1 += exec2, std::invalid_argument);
}

TEST_CASE("Fail isExecutable due to price being larger than the best asking value") {
    OrderBook orderBook;
    auto sellOrder = orderBook.addOrder(sell, 5, 100);
    auto buyOrder = orderBook.addOrder(buy, 100, 99);

    CHECK(orderBook.getBestAsk() == 100);
    CHECK(orderBook.getBestBid() == 99);
}

TEST_CASE("Cancel order more complexly") {
    OrderBook orderBook;
    auto sellOrder = orderBook.addOrder(sell, 5, 100);
    auto buyOrder = orderBook.addOrder(buy, 6, 101);
    auto buyOrder2 = orderBook.addOrder(buy, 9, 101);
    // Here, are 10 for 101, 5 traded
    orderBook.cancelOrder(buyOrder.getBaseId());
    // Now are 9 for 101
    auto buyOrder3 = orderBook.addOrder(buy, 11, 100);
    auto buyOrder4 = orderBook.addOrder(buy, 1, 99);
    auto sellOrder2 = orderBook.addOrder(sell, 21, 99);

    CHECK(!orderBook.getBestBid().has_value());
    CHECK_EQ(orderBook.getTotalVolume(), 26);
    CHECK_EQ(orderBook.getVolumeAtLimit(100), 16);
    CHECK_EQ(orderBook.getVolumeAtLimit(101), 9);
    CHECK_EQ(orderBook.getVolumeAtLimit(99), 1);
    CHECK_EQ(sellOrder2.getMoneyExchanged(), 2108);
    CHECK_EQ(sellOrder2.getTotalSharesExecuted(), 21);
}

TEST_CASE("Final weird case for orderExecution addition") {
    OrderExecution exec1{5};
    OrderExecution exec2{5};

    Order order1{6, buy, 7, 10};
    exec1.executeOrder(order1, 5);
    exec2.executeOrder(order1, 7);
    
    // Doesn't throw because only one of them is a partial order execution
    CHECK_NOTHROW(exec1 += exec2);
}

TEST_CASE("Large test") {
    OrderBook orderBook;
    for(int i = 1; i <= 100; ++i) {
        for(int j = 0; j < 30*i; ++j) {
            orderBook.addOrder(sell, i, 99+i);
        }
    }

    auto buyTheWholeMarket = orderBook.addOrder(buy, 1'000'000'000, 300);

    CHECK_EQ(buyTheWholeMarket.getMoneyExchanged(), 1'769'974'500);

    // 1 1 1 1 x 30 at 100
    // 2 2 2 2 x 60 at 101
    // ...
    // 100 100 100 100 x 3000 at 199
    // Equals $1,769,974,500 
}

TEST_CASE("ID order test") {
    // Make sure first entere orders are first to be executed
    OrderBook orderBook;
    auto buyOrder1 = orderBook.addOrder(buy, 5, 5);
    auto buyOrder2 = orderBook.addOrder(buy, 5, 5);
    auto buyOrder3 = orderBook.addOrder(buy, 5, 4);
    auto buyOrder4 = orderBook.addOrder(buy, 5, 3);
    auto buyOrder5 = orderBook.addOrder(buy, 5, 5);
    auto buyOrder6 = orderBook.addOrder(buy, 5, 4);
    auto buyOrder7 = orderBook.addOrder(buy, 5, 3);

    auto sellOrder1 = orderBook.addOrder(sell, 5, 3);
    auto sellOrder2 = orderBook.addOrder(sell, 5, 3);
    auto sellOrder3 = orderBook.addOrder(sell, 5, 3);
    auto sellOrder4 = orderBook.addOrder(sell, 5, 3);
    auto sellOrder5 = orderBook.addOrder(sell, 5, 3);
    auto sellOrder6 = orderBook.addOrder(sell, 5, 3);
    auto sellOrder7 = orderBook.addOrder(sell, 5, 3);

    CHECK_EQ(buyOrder1.getBaseId(), sellOrder1.getFulfilledOrderIds().front());
    CHECK_EQ(buyOrder2.getBaseId(), sellOrder2.getFulfilledOrderIds().front());
    CHECK_EQ(buyOrder5.getBaseId(), sellOrder3.getFulfilledOrderIds().front());
    CHECK_EQ(buyOrder3.getBaseId(), sellOrder4.getFulfilledOrderIds().front());
    CHECK_EQ(buyOrder6.getBaseId(), sellOrder5.getFulfilledOrderIds().front());
    CHECK_EQ(buyOrder4.getBaseId(), sellOrder6.getFulfilledOrderIds().front());
    CHECK_EQ(buyOrder7.getBaseId(), sellOrder7.getFulfilledOrderIds().front());
}

TEST_SUITE_END();