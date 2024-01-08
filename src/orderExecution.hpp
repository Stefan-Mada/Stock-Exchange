/**
 * @file orderExecution.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief File defining class that stores information about order executions
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ORDEREXECUTION_HPP
#define ORDEREXECUTION_HPP

#include <optional>
#include <utility>
#include <vector>

namespace Exchange {

// rather than including "order.hpp", which would cause a cyclic dependency, just declare class here
struct Order;


/**
 * @brief Represents all orders executed to fulfill one submitted order (the base order)
 * 
 */
struct OrderExecution {
    /**
     * @brief Construct a new OrderExecution object
     * 
     * @param baseId The ID of the order tied to this OrderExecution
     */
    explicit OrderExecution(int baseId) : baseId{baseId} {}

    /**
     * @brief Adds another order to this OrderExecution, appending all fulfilled orders, summing money Exchanged, etc
     * 
     * @param rhs OrderExecution object to sum to this OrderExecution
     * @return Reference to this OrderExecution object
     */
    auto operator+=(const OrderExecution &rhs) -> OrderExecution &;

    /**
     * @brief Adds number of shares from order to this OrderExecution, or all shares in the order if shares > order.shares
     * 
     * @param order     Order to add to this execution
     * @param shares    Number of shares to execute from that order
     */
    void executeOrder(const Order& order, int shares);

    /**
     * @brief Get the total shares executed by this execution
     * 
     * @return Number of shares
     */
    [[nodiscard]] auto getTotalSharesExecuted() const -> int;

    /**
     * @brief Get if this execution has partially executed any order
     * 
     * @return True if partial execution occurred, false otherwise
     */
    [[nodiscard]] auto hasPartialExecution() const -> bool;

    /**
     * @brief Get the id of the Order tied to this OrderExecution
     * 
     * @return orderId
     */
    [[nodiscard]] auto getBaseId() const -> int;

    /**
     * @brief Get the amount of money exchanged by this execution
     * 
     * @return Dollar amount
     */
    [[nodiscard]] auto getMoneyExchanged() const -> int;

    /**
     * @brief Get a vector all orders FULLY fulfilled by this object (no partial executions)
     * 
     * @return Vector of fully fulfilled order Ids
     */
    [[nodiscard]] auto getFulfilledOrderIds() const -> const std::vector<int> &;

    /**
     * @brief Get information about the partially fulfilled order, if any
     * 
     * @return Pair of orderId and number of shares executed by the partial execution, or std::nullopt if no partial execution
     */
    [[nodiscard]] auto getPartiallyFulfilledOrder() const
        -> const std::optional<std::pair<int, int>> &;

  private:
    /**
     * @brief Fully executes the given order
     * 
     * @param order Order to fully execute
     */
    void executeOrder(const Order& order);

    /// @brief ID of order that is being executed to start with
    int baseId;
    int moneyExchanged = 0;
    int totalSharesExcecuted = 0;
    std::vector<int> fulfilledOrderIds;

    /// @brief Pair of orderId, and number of shares executed
    std::optional<std::pair<int, int>> partiallyFulfilledOrder;
};

} // namespace Exchange

#endif