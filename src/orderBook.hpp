/**
 * @file orderBook.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Header file for the limit order book data structure
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include "limitPrice.hpp"
#include <map>
#include <optional>
#include <unordered_map>

namespace Exchange {

/**
 * @brief Limit order book data structure
 *  
 * Holds buy and sell trees of limit prices, as well as unordered maps
 * mapping IDs to orders, and prices to limits.
 */
struct OrderBook {
    OrderBook() = default;

    auto addOrder(OrderType orderType, int shares, int limitPrice,
                  int timeInForce = 0) -> OrderExecution;
    void cancelOrder(int orderId);
    auto getVolumeAtLimit(int price) const -> int;
    auto getBestBid() const -> std::optional<int>;
    auto getBestAsk() const -> std::optional<int>;
    auto getTotalVolume() const -> int;

  private:
    auto addOrder(const Order &order) -> OrderExecution;
    auto executeOrder(const Order &order) -> OrderExecution;
    auto isExecutable(const Order &order) const -> bool;
    void removeLimitMap(int price, OrderType orderType);

    std::map<int, LimitPrice> buyMap;
    std::map<int, LimitPrice> sellMap;

    // This is used when no more orders exist in a LimitPrice,
    // Necessary in order to keep storing information about volume, etc
    std::unordered_map<int, LimitPrice> archivedLimitMaps;

    std::unordered_map<int, std::list<Order>::iterator> idToOrderIteratorMap;
    
    /// @brief Can avoid having 2 maps for buying and selling, as the ranges should never overlap
    std::unordered_map<int, LimitPrice&> priceToLimitMap;

    int totalVolume = 0;
    int currentOrderId = 0;
};

} // namespace Exchange

#endif