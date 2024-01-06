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

namespace Exchange {

OrderExecution& OrderExecution::operator+=(const OrderExecution& rhs) {
    if(baseId != rhs.baseId)
        throw std::invalid_argument("BaseID of added OrderExecutions must be equal");
    if(partiallyFulfilledOrder && rhs.partiallyFulfilledOrder)
        throw std::invalid_argument("2 added OrderExecutions can't both have partially fulfilled orders");

    fulfilledOrderIds.insert(fulfilledOrderIds.end(), rhs.fulfilledOrderIds.begin(), rhs.fulfilledOrderIds.end());

    currProfit += rhs.currProfit;
    totalSharesExcecuted += rhs.totalSharesExcecuted;

    return *this;
}

void OrderExecution::executeOrder(const Order& order) {
    currProfit += order.getLimitPrice() * order.getShares();
    fulfilledOrderIds.push_back(order.getOrderId());
    totalSharesExcecuted += order.getShares();
}

void OrderExecution::executeOrder(const Order& order, int shares) {
    if(shares >= order.getShares())
        executeOrder(order);
    else {
        currProfit += order.getLimitPrice() * shares;
        partiallyFulfilledOrder = {order.getOrderId(), shares};
        totalSharesExcecuted += shares;
    }
}

int OrderExecution::getTotalSharesExecuted() const {
    return totalSharesExcecuted;
}



}