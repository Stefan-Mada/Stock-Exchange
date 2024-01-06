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
    Order& getFirstOrder() const;
    int getVolume() const;
    int getPrice() const;

private:
    int limitPrice;
    int depth = 0;
    int volume = 0;
    std::list<Order> limitOrders;
};

};

#endif