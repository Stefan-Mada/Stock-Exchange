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

namespace Exchange {

std::list<Order>::iterator LimitPrice::addOrder(const Order& order) {
    limitOrders.emplace_back(order);
    depth += order.getShares();
    return --limitOrders.end();
}

void LimitPrice::removeOrder(std::list<Order>::const_iterator pos) {
    depth -= pos->getShares();
    limitOrders.erase(pos);
}

bool LimitPrice::isEmpty() const {
    return depth == 0;
}

const Order& LimitPrice::getFirstOrder() const {
    return limitOrders.front();
}

int LimitPrice::getVolume() const {
    return volume;
}

int LimitPrice::getPrice() const {
    return limitPrice;
}

OrderExecution LimitPrice::executeNumberOfShares(int baseOrderId, int numShares) {
    if(numShares > depth)
        throw std::invalid_argument("Can't execute more shares in LimitPrice than exist in depth");

    OrderExecution totalOrderExecution(baseOrderId);

    while(numShares > 0) {
        OrderExecution firstOrderExec = limitOrders.front().execute(baseOrderId, numShares);
        numShares -= firstOrderExec.getTotalSharesExecuted();
        totalOrderExecution += firstOrderExec;
    }

    return totalOrderExecution;
}

}