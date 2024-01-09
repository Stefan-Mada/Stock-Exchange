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
    /**
     * @brief Construct an empty OrderBook object
     * 
     */
    OrderBook() = default;

    /**
     * @brief Adds a new order to the OrderBook
     * 
     * @param orderType     Buy or sell
     * @param shares        Number of shares
     * @param limitPrice    Price of Order
     * @param timeInForce   Time until order expires TODO: Make meaningful
     * @return OrderExecution, containing order's unique ID, and info about any orders executed by adding this order
     */
    auto addOrder(OrderAction orderType, int shares, int limitPrice,
                  int timeInForce = 0) -> OrderExecution;

    /**
     * @brief Cancel order with given orderId
     * 
     * @param orderId OrderId to cancel
     * @warning Doesn't check to make sure orderId exists
     */
    void cancelOrder(int orderId);

    /**
     * @brief Get the volume at a specific limit price
     * 
     * @param price LimitPrice to check
     * @return Number of shares traded at a given limit
     */
    [[nodiscard]] auto getVolumeAtLimit(int price) const -> int;

    /**
     * @brief Get the best bidding price
     * 
     * @return Best bid (buying) price available, or std::nullopt if no buy order available
     */
    [[nodiscard]] auto getBestBid() const -> std::optional<int>;

    /**
     * @brief Get the best asking price
     * 
     * @return Best ask (selling) price available, or std::nullopt if no sell order available
     */
    [[nodiscard]] auto getBestAsk() const -> std::optional<int>;

    /**
     * @brief Get the total volume traded in this orderBook
     * 
     * @return Number of shares traded in orderBook
     */
    [[nodiscard]] auto getTotalVolume() const -> int;

  private:
    /**
     * @brief Adds an order given an order object
     * 
     * @param order Order to add to orderBook
     * @return OrderExecution, containing order's unique ID, and info about any orders executed by adding this order 
     */
    auto addOrder(const Order &order) -> OrderExecution;

    /**
     * @brief Execute a given order against the existing orderBook
     * 
     * @param order To execute in orderBook
     * @return OrderExecution, containing order's unique ID, and info about any orders executed by this order  
     */
    auto executeOrder(const Order &order) -> OrderExecution;

    /**
     * @brief Check if an order is currently able to execute another order, given the state of the orderBook
     * 
     * @param order Order to check against existing OrderBook
     * @return True if an existing order can fulfill the order being passed in, false otherwise 
     */
    [[nodiscard]] auto isExecutable(const Order &order) const -> bool;

    /**
     * @brief Remove a limitMap from the active buy/sell maps and archive it
     * 
     * @param price         Price of limitMap to delete
     * @param orderType     Whether it is a buying or selling limitMap
     * @warning Doesn't check to see if limitMap exists in the buy/sell Map
     */
    void removeLimitMap(int price, OrderAction orderType);

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