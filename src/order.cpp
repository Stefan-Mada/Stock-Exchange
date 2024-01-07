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

OrderType Order::getOrderType() const {
    return orderType;
}

int Order::getLimitPrice() const {
    return limitPrice;
}

int Order::getOrderId() const {
    return orderId;
}

int Order::getShares() const {
    return shares;
}

int Order::getTimeInForce() const {
    return timeInForce;
}

OrderExecution Order::execute(int baseOrderId, int numShares) {
    OrderExecution orderExecution{baseOrderId};
    orderExecution.executeOrder(*this, numShares);
    shares = std::max(0, shares - numShares); // either 0 for full execution, or a partial amount remaining

    return orderExecution;
}

Order& Order::updateShareCount(const int newShares) {
    shares = newShares;
    return *this;
}

Order Order::copyWithNewShareCount(const int newShares) const {
    Order newOrder(*this);
    newOrder.shares = newShares;
    return newOrder;
}

}

