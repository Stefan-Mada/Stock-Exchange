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
enum OrderType {
    buy,
    sell
};

/**
 * @brief The Order struct, which holds all necessary information for a single order
 * 
 */
struct Order {
    Order(int orderId, OrderType orderType, int shares, int limitPrice, int timeInForce = 0)
        : orderId{orderId}, orderType{orderType}, shares{shares}, limitPrice{limitPrice}, timeInForce{timeInForce} {}

    [[nodiscard]] auto getOrderType() const -> OrderType;
    [[nodiscard]] auto getLimitPrice() const -> int;
    [[nodiscard]] auto getOrderId() const -> int;
    [[nodiscard]] auto getShares() const -> int;
    [[nodiscard]] auto getTimeInForce() const -> int;

    /**
     * @brief Fills some or all of the shares of this order. Modifies this order object.
     * 
     * @param baseOrderId The base order that is being fulfilled by this order
     * @param numShares Number of shares executed
     * @return Order Execution with this order fulfilled, partially or fully
     */
    auto execute(int baseOrderId, int numShares) -> OrderExecution;

    [[nodiscard]] auto copyWithNewShareCount(const int newShares) const
        -> Order;

  private:
    int orderId;
    OrderType orderType;
    int shares;
    int limitPrice;

    // TODO: Do this.
    /// @brief Currently does nothing, but will hopefully in the future be used to remove expired orders. TIF = 0 is indefinite
    int timeInForce;
};

} // namespace Exchange

#endif