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

#include <list>
#include "order.hpp"

namespace Exchange {

/**
 * @brief The Limit Price struct, which holds information for all orders at a given limit price
 * 
 */
struct LimitPrice {
    LimitPrice(int limitPrice) : limitPrice{limitPrice} {}


    std::list<Order>::iterator addOrder(const Order& order);
    void removeOrder(std::list<Order>::const_iterator pos);
    bool isEmpty() const;
    const Order& getFirstOrder() const;
    int getVolume() const;
    int getPrice() const;

    /**
     * @brief Will execute a certain number of shares at this price, modifying orders, but not deleting them.
     * 
     * @param baseOrderId The base order that is trying to be filled here
     * @param numShares Number of shares to fulfill
     * @return OrderExecution object with shares executed information
     * @warning Will not accept more shares than exist depth in the limit
     */
    OrderExecution executeNumberOfShares(int baseOrderId, int numShares);

private:
    int limitPrice;
    int depth = 0;
    int volume = 0;
    std::list<Order> limitOrders;
};

};

#endif