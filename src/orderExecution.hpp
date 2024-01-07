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
    OrderExecution(int baseId) : baseId{baseId} {}

    auto operator+=(const OrderExecution &rhs) -> OrderExecution &;

    void executeOrder(const Order& order, int shares);

    [[nodiscard]] auto getTotalSharesExecuted() const -> int;

    [[nodiscard]] auto hasPartialExecution() const -> bool;

    [[nodiscard]] auto getBaseId() const -> int;

    [[nodiscard]] auto getMoneyExchanged() const -> int;

    [[nodiscard]] auto getFulfilledOrderIds() const -> const std::vector<int> &;

    [[nodiscard]] auto getPartiallyFulfilledOrder() const
        -> const std::optional<std::pair<int, int>> &;

  private:
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