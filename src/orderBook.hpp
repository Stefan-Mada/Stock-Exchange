/**
 * @file orderBook.hpp
 * @author Stefan Mada
 * @brief Header file for the limit order book data structure
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <map>
#include <optional>
#include <unordered_map>
#include "limitPrice.hpp"

namespace Exchange {

/**
 * @brief Limit order book data structure
 *  
 * Holds buy and sell trees of limit prices, as well as unordered maps
 * mapping IDs to orders, and prices to limits.
 */
struct OrderBook {
    OrderBook() = default;

    std::optional<int> addOrder(const Order& order);
    void cancelOrder(int orderId);
    int getVolumeAtLimit(int price);
    int getBestBid();
    int getBestAsk();
    int getTotalVolume();

private:
    int executeOrder(const Order& order);
    int fulfillOrder(Order& orderToFulfill, int numSharesExecuted);

    std::map<int, LimitPrice> buyMap;
    std::map<int, LimitPrice> sellMap;

    std::unordered_map<int, std::list<Order>::iterator> idToOrderIteratorMap;
    
    /// @brief Can avoid having 2 maps for buying and selling, as the ranges should never overlap
    std::unordered_map<int, LimitPrice> priceToLimitMap;

    int totalVolume = 0;
    int currentOrderId = 0;
};

};

#endif