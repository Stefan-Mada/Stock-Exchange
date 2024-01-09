/**
 * @file order.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Header file for the struct representing a single limit order
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ORDER_HPP
#define ORDER_HPP

#include "orderExecution.hpp"

namespace Exchange {

/**
 * @brief Order types possible for an order: currently buy or sell
 * 
 */
enum OrderAction {
    buy,
    sell
};

/**
 * @brief The Order struct, which holds all necessary information for a single order
 * 
 */
struct Order {
    /**
     * @brief Construct a new Order object
     * 
     * @param orderId       Unique id of order
     * @param orderType     Buy or sell
     * @param shares        Number of shares in order
     * @param limitPrice    Price for order
     * @param timeInForce   Time the order is valid for
     */
    Order(int orderId, OrderAction orderType, int shares, int limitPrice, int timeInForce = 0)
        : orderId{orderId}, orderType{orderType}, shares{shares}, limitPrice{limitPrice}, timeInForce{timeInForce} {}

    /**
     * @brief Get the type of order
     * 
     * @return OrderType (buy or sell)
     */
    [[nodiscard]] auto getOrderType() const -> OrderAction;

    /**
     * @brief Get the limit price of this order
     * 
     * @return Order's price
     */
    [[nodiscard]] auto getLimitPrice() const -> int;

    /**
     * @brief Get the id of order
     * 
     * @return Order's id
     */
    [[nodiscard]] auto getOrderId() const -> int;

    /**
     * @brief Get the number of shares in the order
     * 
     * @return Number of shares
     */
    [[nodiscard]] auto getShares() const -> int;

    /**
     * @brief Get the time in force of this order
     * TODO: Make time in force a meaningful number
     * @return Time in force
     */
    [[nodiscard]] auto getTimeInForce() const -> int;

    /**
     * @brief Fills some or all of the shares of this order. Modifies this order object.
     * 
     * @param baseOrderId The base order that is being fulfilled by this order
     * @param numShares Number of shares executed
     * @return OrderExecution with this order fulfilled, partially or fully
     */
    auto execute(int baseOrderId, int numShares) -> OrderExecution;

    /**
     * @brief Copy this Order object, but set a new sharecount in the new Order
     * 
     * @param newShares Number of shares to set in copied Order
     * @return New Order
     */
    [[nodiscard]] auto copyWithNewShareCount(const int newShares) const
        -> Order;

  private:
    int orderId;
    OrderAction orderType;
    int shares;
    int limitPrice;

    // TODO: Do this.
    /// @brief Currently does nothing, but will hopefully in the future be used to remove expired orders. TIF = 0 is indefinite
    int timeInForce;
};

} // namespace Exchange

#endif