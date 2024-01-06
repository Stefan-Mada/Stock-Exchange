/**
 * @file orderBook.cpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Implements order book member functions
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "orderBook.hpp"

namespace Exchange {

std::optional<OrderExecution> OrderBook::addOrder(const Order& order) {
    const auto orderPrice = order.getLimitPrice();
    const auto orderId = order.getOrderId();
    auto& buyOrSellMap = (order.getOrderType() == OrderType::buy) ? buyMap : sellMap;

    if(isExecutable(order))
        return executeOrder(order);

    auto [limitPriceIterator, isNewElem] = buyOrSellMap.try_emplace(orderPrice, orderPrice);
    auto orderIterator = limitPriceIterator->second.addOrder(order);
    idToOrderIteratorMap.insert({orderId, orderIterator});

    if(isNewElem)
        priceToLimitMap.insert({orderPrice, limitPriceIterator->second});


    return {};
}

void OrderBook::cancelOrder(int orderId) {
    const auto& orderIterator = idToOrderIteratorMap.at(orderId);
    const int price = orderIterator->getLimitPrice();

    LimitPrice& limitPrice = priceToLimitMap.at(price);
    limitPrice.removeOrder(orderIterator);
    idToOrderIteratorMap.erase(orderId);

    // could check if limitPrice is empty here and delete accordingly,
    // but would add O(logn) complexity unnecessarily - can just keep empty limitPrices
}

OrderExecution OrderBook::executeOrder(const Order& order) {
    const int startingShares = order.getShares();
    const int baseOrderId = order.getOrderId();
    int sharesLeftToExec = startingShares;

    OrderExecution totalExec{baseOrderId};

    while(sharesLeftToExec > 0 && isExecutable(order)) {
        LimitPrice& targetLimit = (order.getOrderType() == OrderType::buy) ? buyMap.begin()->second : (--sellMap.end())->second;

        const int sharesLeftToExec = startingShares - totalExec.getTotalSharesExecuted();
        const int sharesToExecInLimit = std::min(sharesLeftToExec, targetLimit.getDepth());
        OrderExecution limitExecution = targetLimit.executeNumberOfShares(baseOrderId, sharesToExecInLimit);

        for(const auto fulfilledId : limitExecution.getFulfilledOrderIds())
            idToOrderIteratorMap.erase(fulfilledId);

        totalExec += limitExecution;
    }

    if(sharesLeftToExec > 0) 
        addOrder(order.copyWithNewShareCount(sharesLeftToExec));

    return totalExec;
}

int OrderBook::getVolumeAtLimit(int price) const {
    return priceToLimitMap.at(price).getVolume();
}

int OrderBook::getBestBid() const {
    return std::prev(buyMap.end())->second.getPrice();
}

int OrderBook::getBestAsk() const {
    return sellMap.begin()->second.getPrice();
}

int OrderBook::getTotalVolume() const {
    return totalVolume;
}

bool OrderBook::isExecutable(const Order& order) const {
    const auto orderType = order.getOrderType();
    const auto price = order.getLimitPrice();

    if(orderType == OrderType::buy && price >= getBestAsk())
        return true;
    else if(orderType == OrderType::sell && price <= getBestBid())
        return true;
    
    return false;
}

};