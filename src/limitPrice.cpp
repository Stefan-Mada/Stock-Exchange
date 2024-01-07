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

#include <stdexcept>
#include "limitPrice.hpp"

namespace Exchange {

std::list<Order>::iterator LimitPrice::addOrder(const Order& order) {
    limitOrders.emplace_back(order);
    depth += order.getShares();
    return --limitOrders.end();
}

OrderType LimitPrice::removeOrder(std::list<Order>::const_iterator pos) {
    OrderType type = pos->getOrderType();
    depth -= pos->getShares();
    limitOrders.erase(pos);

    return type;
}

bool LimitPrice::isEmpty() const {
    return depth == 0;
}

int LimitPrice::getVolume() const {
    return volume;
}

int LimitPrice::getPrice() const {
    return limitPrice;
}

int LimitPrice::getDepth() const {
    return depth;
}

OrderExecution LimitPrice::executeNumberOfShares(int baseOrderId, int numShares) {
    if(numShares > depth)
        throw std::invalid_argument("Can't execute more shares in LimitPrice than exist in depth");

    OrderExecution totalOrderExecution(baseOrderId);

    while(numShares > 0) {
        Order& frontOrder = limitOrders.front();
        OrderExecution firstOrderExec = frontOrder.execute(baseOrderId, numShares);
        numShares -= firstOrderExec.getTotalSharesExecuted();
        totalOrderExecution += firstOrderExec;

        if(frontOrder.getShares() == 0)
            limitOrders.pop_front();
    }

    volume += totalOrderExecution.getTotalSharesExecuted();
    depth -= totalOrderExecution.getTotalSharesExecuted();

    return totalOrderExecution;
}

}