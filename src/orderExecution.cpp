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

#include <stdexcept>
#include "orderExecution.hpp"
#include "order.hpp"

namespace Exchange {

OrderExecution& OrderExecution::operator+=(const OrderExecution& rhs) {
    if(baseId != rhs.baseId)
        throw std::invalid_argument("BaseID of added OrderExecutions must be equal");
    if(partiallyFulfilledOrder && rhs.partiallyFulfilledOrder)
        throw std::invalid_argument("2 added OrderExecutions can't both have partially fulfilled orders");

    fulfilledOrderIds.insert(fulfilledOrderIds.end(), rhs.fulfilledOrderIds.begin(), rhs.fulfilledOrderIds.end());

    moneyExchanged += rhs.moneyExchanged;
    totalSharesExcecuted += rhs.totalSharesExcecuted;

    if(rhs.partiallyFulfilledOrder)
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

int OrderExecution::getTotalSharesExecuted() const {
    return totalSharesExcecuted;
}

bool OrderExecution::hasPartialExecution() const {
    return partiallyFulfilledOrder.has_value();
}

int OrderExecution::getBaseId() const {
    return baseId;
}

int OrderExecution::getMoneyExchanged() const {
    return moneyExchanged;
}

const std::vector<int>& OrderExecution::getFulfilledOrderIds() const {
    return fulfilledOrderIds;
}

const std::optional<std::pair<int, int>>& OrderExecution::getPartiallyFulfilledOrder() const {
    return partiallyFulfilledOrder;
}



}