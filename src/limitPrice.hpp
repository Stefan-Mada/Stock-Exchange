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
    /**
     * @brief Construct a new Limit Price object
     * 
     * @param limitPrice Price of the limitPrice object to keep track off
     */
    explicit LimitPrice(int limitPrice) : limitPrice{limitPrice} {}
    
    /**
     * @brief Adds an order to the end of the given limitPrice
     * 
     * @param order Order object to insert into this limitPrice
     * @return std::list<Order>::iterator to inserted object
     * @throws std::invalid_argument if order object's price is not equal to this limitPrice
     */
    auto addOrder(const Order &order) -> std::list<Order>::iterator;

    /**
     * @brief Removes an order from the limitPrice object
     * 
     * @param pos Iterator to the position to remove
     * @return The type of the order removed.
     * @warning Doesn't check to ensure iterator is valid
     */
    auto removeOrder(std::list<Order>::const_iterator pos) -> OrderAction;

    /**
     * @brief Check if limitPrice has any orders in it
     * 
     * @return True if no orders, false otherwise 
     */
    [[nodiscard]] auto isEmpty() const -> bool;

    /**
     * @brief Get the volume of all transactions that have occurred in shares
     * 
     * @return Number of shares 
     */
    [[nodiscard]] auto getVolume() const -> int;

    /**
     * @brief Get the price of this LimitPrice object
     * 
     * @return This LimitPrice's price
     */
    [[nodiscard]] auto getPrice() const -> int;

    /**
     * @brief Get the number of shares available in this limitPrice
     * 
     * @return Number of shares 
     */
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