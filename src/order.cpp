/**
 * @file order.cpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Implements order class member functions
 * @version 1.0
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "order.hpp"

namespace Exchange {

auto Order::getOrderType() const -> OrderType { return orderType; }

auto Order::getLimitPrice() const -> int { return limitPrice; }

auto Order::getOrderId() const -> int { return orderId; }

auto Order::getShares() const -> int { return shares; }

auto Order::getTimeInForce() const -> int { return timeInForce; }

auto Order::execute(int baseOrderId, int numShares) -> OrderExecution {
  OrderExecution orderExecution{baseOrderId};
  orderExecution.executeOrder(*this, numShares);
  shares = std::max(0, shares - numShares); // either 0 for full execution, or a
                                            // partial amount remaining

  return orderExecution;
}

auto Order::copyWithNewShareCount(const int newShares) const -> Order {
  Order newOrder(*this);
  newOrder.shares = newShares;
  return newOrder;
}

} // namespace Exchange
