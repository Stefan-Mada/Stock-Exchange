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

OrderExecution OrderBook::addOrder(OrderType orderType, int shares, int limitPrice, int timeInForce) {
    return addOrder(Order{currentOrderId++, orderType, shares, limitPrice, timeInForce});
}

OrderExecution OrderBook::addOrder(const Order& order) {
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


    // if order is simply added without executing, return an empty order execution with the ID of the order
    return OrderExecution(order.getOrderId());
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
        LimitPrice& targetLimit = (order.getOrderType() == OrderType::sell) ? buyMap.begin()->second : (--sellMap.end())->second;
         
        sharesLeftToExec = startingShares - totalExec.getTotalSharesExecuted();
        const int sharesToExecInLimit = std::min(sharesLeftToExec, targetLimit.getDepth());

        OrderExecution limitExecution = targetLimit.executeNumberOfShares(baseOrderId, sharesToExecInLimit);

        for(const auto fulfilledId : limitExecution.getFulfilledOrderIds())
            idToOrderIteratorMap.erase(fulfilledId);

        totalExec += limitExecution;
    }

    if(sharesLeftToExec > 0) 
        addOrder(order.copyWithNewShareCount(sharesLeftToExec));

    totalVolume += totalExec.getTotalSharesExecuted();

    return totalExec;
}

int OrderBook::getVolumeAtLimit(int price) const {
    if(!priceToLimitMap.contains(price))
        return 0;

    return priceToLimitMap.at(price).getVolume();
}

std::optional<int> OrderBook::getBestBid() const {
    if(buyMap.empty())
        return {};

    return std::prev(buyMap.end())->second.getPrice();
}

std::optional<int> OrderBook::getBestAsk() const {
    if(sellMap.empty())
        return {};

    return sellMap.begin()->second.getPrice();
}

int OrderBook::getTotalVolume() const {
    return totalVolume;
}

bool OrderBook::isExecutable(const Order& order) const {
    const auto orderType = order.getOrderType();
    const auto price = order.getLimitPrice();

    auto bestAsk = getBestAsk();
    auto bestBid = getBestBid();

    if(orderType == OrderType::buy && bestAsk && price >= bestAsk.value())
        return true;
    else if(orderType == OrderType::sell && bestBid && price <= bestBid)
        return true;
    
    return false;
}

};