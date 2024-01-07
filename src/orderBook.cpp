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

    // if the limit existed in the past, we need to re-instate it to keep track of volume, etc.
    if(archivedLimitMaps.contains(orderPrice)) {
        buyOrSellMap.emplace(orderPrice, archivedLimitMaps.at(orderPrice));
        archivedLimitMaps.erase(orderPrice);
    }
    // Can continue as normal adding to the LimitPrice
    auto [limitPriceIterator, isNewElem] = buyOrSellMap.try_emplace(orderPrice, orderPrice);
    auto orderIterator = limitPriceIterator->second.addOrder(order);
    idToOrderIteratorMap.insert({orderId, orderIterator});

    priceToLimitMap.insert_or_assign(orderPrice, limitPriceIterator->second);


    // if order is simply added without executing, return an empty order execution with the ID of the order
    return OrderExecution(order.getOrderId());
}

void OrderBook::cancelOrder(int orderId) {
    const auto& orderIterator = idToOrderIteratorMap.at(orderId);
    const int price = orderIterator->getLimitPrice();

    LimitPrice& limitPrice = priceToLimitMap.at(price);
    OrderType removedType = limitPrice.removeOrder(orderIterator);
    idToOrderIteratorMap.erase(orderId);

    // Need to erase empty limitPrices here, as gives incorrect info on lowest bids/asks
    // TODO: See if can do better than O(logn) for cancelling orders in empty case
    if(limitPrice.isEmpty())
        removeLimitMap(price, removedType);
}

OrderExecution OrderBook::executeOrder(const Order& order) {
    const int startingShares = order.getShares();
    const int baseOrderId = order.getOrderId();
    int sharesLeftToExec = startingShares;

    OrderExecution totalExec{baseOrderId};

    while(sharesLeftToExec > 0 && isExecutable(order)) {
        OrderType targetLimitType = (order.getOrderType() == OrderType::sell) ? OrderType::buy : OrderType::sell;
        LimitPrice& targetLimit = (order.getOrderType() == OrderType::sell) ? (--buyMap.end())->second : sellMap.begin()->second;
        
        const int sharesToExecInLimit = std::min(sharesLeftToExec, targetLimit.getDepth());

        OrderExecution limitExecution = targetLimit.executeNumberOfShares(baseOrderId, sharesToExecInLimit);

        for(const auto fulfilledId : limitExecution.getFulfilledOrderIds())
            idToOrderIteratorMap.erase(fulfilledId);

        if(targetLimit.isEmpty())
            removeLimitMap(targetLimit.getPrice(), targetLimitType);

        totalExec += limitExecution;
        sharesLeftToExec = startingShares - totalExec.getTotalSharesExecuted();
    }

    if(sharesLeftToExec > 0) 
        addOrder(order.copyWithNewShareCount(sharesLeftToExec));

    totalVolume += totalExec.getTotalSharesExecuted();

    return totalExec;
}

int OrderBook::getVolumeAtLimit(int price) const {
    int volumeAtLimit = 0;
    if(priceToLimitMap.contains(price))
        volumeAtLimit += priceToLimitMap.at(price).getVolume();
    if(archivedLimitMaps.contains(price))
        volumeAtLimit += archivedLimitMaps.at(price).getVolume();

    return volumeAtLimit;
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

void OrderBook::removeLimitMap(int price, OrderType orderType) {
    if(orderType == OrderType::buy) {
        archivedLimitMaps.emplace(price, priceToLimitMap.at(price));
        buyMap.erase(price);
    }
    else {
        archivedLimitMaps.emplace(price, priceToLimitMap.at(price));
        sellMap.erase(price);
    }

    priceToLimitMap.erase(price);
}

};