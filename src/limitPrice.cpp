/**
 * @file limitPrice.cpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Implements LimitPrice class
 * @version 1.0
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "limitPrice.hpp"
#include <stdexcept>

namespace Exchange {

auto LimitPrice::addOrder(const Order &order) -> std::list<Order>::iterator {
  limitOrders.emplace_back(order);
  depth += order.getShares();
  return --limitOrders.end();
}

auto LimitPrice::removeOrder(std::list<Order>::const_iterator pos)
    -> OrderType {
  OrderType type = pos->getOrderType();
  depth -= pos->getShares();
  limitOrders.erase(pos);

  return type;
}

auto LimitPrice::isEmpty() const -> bool { return depth == 0; }

auto LimitPrice::getVolume() const -> int { return volume; }

auto LimitPrice::getPrice() const -> int { return limitPrice; }

auto LimitPrice::getDepth() const -> int { return depth; }

auto LimitPrice::executeNumberOfShares(int baseOrderId, int numShares)
    -> OrderExecution {
  if (numShares > depth)
    throw std::invalid_argument(
        "Can't execute more shares in LimitPrice than exist in depth");

  OrderExecution totalOrderExecution(baseOrderId);

  while (numShares > 0) {
    Order &frontOrder = limitOrders.front();
    OrderExecution firstOrderExec = frontOrder.execute(baseOrderId, numShares);
    numShares -= firstOrderExec.getTotalSharesExecuted();
    totalOrderExecution += firstOrderExec;

    if (frontOrder.getShares() == 0)
      limitOrders.pop_front();
  }

  volume += totalOrderExecution.getTotalSharesExecuted();
  depth -= totalOrderExecution.getTotalSharesExecuted();

  return totalOrderExecution;
}

} // namespace Exchange