/**
 * @file orderExecution.cpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Implement the member functions of OrderExecution
 * @version 1.0
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "orderExecution.hpp"
#include "order.hpp"
#include <stdexcept>

namespace Exchange {

auto OrderExecution::operator+=(const OrderExecution &rhs) -> OrderExecution & {
  if (baseId != rhs.baseId)
    throw std::invalid_argument(
        "BaseID of added OrderExecutions must be equal");
  if (partiallyFulfilledOrder && rhs.partiallyFulfilledOrder)
    throw std::invalid_argument(
        "2 added OrderExecutions can't both have partially fulfilled orders");

  fulfilledOrderIds.insert(fulfilledOrderIds.end(),
                           rhs.fulfilledOrderIds.begin(),
                           rhs.fulfilledOrderIds.end());

  moneyExchanged += rhs.moneyExchanged;
  totalSharesExcecuted += rhs.totalSharesExcecuted;

  if (rhs.partiallyFulfilledOrder)
    partiallyFulfilledOrder = rhs.partiallyFulfilledOrder;

  return *this;
}

void OrderExecution::executeOrder(const Order& order) {
    moneyExchanged += order.getLimitPrice() * order.getShares();
    fulfilledOrderIds.push_back(order.getOrderId());
    totalSharesExcecuted += order.getShares();
}

void OrderExecution::executeOrder(const Order& order, int shares) {
    if(shares >= order.getShares())
        executeOrder(order);
    else {
        moneyExchanged += order.getLimitPrice() * shares;
        partiallyFulfilledOrder = {order.getOrderId(), shares};
        totalSharesExcecuted += shares;
    }
}

auto OrderExecution::getTotalSharesExecuted() const -> int {
  return totalSharesExcecuted;
}

auto OrderExecution::hasPartialExecution() const -> bool {
  return partiallyFulfilledOrder.has_value();
}

auto OrderExecution::getBaseId() const -> int { return baseId; }

auto OrderExecution::getMoneyExchanged() const -> int { return moneyExchanged; }

auto OrderExecution::getFulfilledOrderIds() const -> const std::vector<int> & {
  return fulfilledOrderIds;
}

auto OrderExecution::getPartiallyFulfilledOrder() const
    -> const std::optional<std::pair<int, int>> & {
  return partiallyFulfilledOrder;
}

} // namespace Exchange