/**
 * @file limitPrice.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Header file for the limit price data structure
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef LIMITPRICE_HPP
#define LIMITPRICE_HPP

#include "order.hpp"
#include <list>

namespace Exchange {

/**
 * @brief The Limit Price struct, which holds information for all orders at a given limit price
 * 
 */
struct LimitPrice {
    LimitPrice(int limitPrice) : limitPrice{limitPrice} {}

    auto addOrder(const Order &order) -> std::list<Order>::iterator;
    auto removeOrder(std::list<Order>::const_iterator pos) -> OrderType;
    [[nodiscard]] auto isEmpty() const -> bool;
    [[nodiscard]] auto getVolume() const -> int;
    [[nodiscard]] auto getPrice() const -> int;
    [[nodiscard]] auto getDepth() const -> int;

    /**
     * @brief Will execute a certain number of shares at this price, modifying orders, and deleting fully executed orders.
     * 
     * @param baseOrderId The base order that is trying to be filled here
     * @param numShares Number of shares to fulfill
     * @return OrderExecution object with shares executed information
     * @warning Will not accept more shares than exist depth in the limit
     */
    auto executeNumberOfShares(int baseOrderId, int numShares)
        -> OrderExecution;

  private:
    int limitPrice;
    int depth = 0;
    int volume = 0;
    std::list<Order> limitOrders;
};

} // namespace Exchange

#endif