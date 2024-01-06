/**
 * @file orderBook.cpp
 * @author Stefan Mada
 * @brief Implements order book member functions
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "orderBook.hpp"

namespace Exchange {

std::optional<int> OrderBook::addOrder(const Order& order) {
    const auto orderPrice = order.getLimitPrice();
    const auto orderId = order.getOrderId();

    switch(order.getOrderType()) {
        case OrderType::buy: {
            if(orderPrice >= getBestAsk())
                return executeOrder(order);

            if(priceToLimitMap.contains(orderPrice)) {
                auto orderIterator = priceToLimitMap[orderPrice].addOrder(order);
                idToOrderIteratorMap[orderId] = orderIterator;
            }
            else {
                buyMap[orderPrice] = {LimitPrice{orderPrice}};
                idToOrderIteratorMap[orderId] = buyMap[orderPrice].addOrder(order);
            }
            break;
        }
        case OrderType::sell: {
            if(orderPrice <= getBestBid())
                return executeOrder(order);

            if(priceToLimitMap.contains(orderPrice)) {
                auto orderIterator = priceToLimitMap[orderPrice].addOrder(order);
                idToOrderIteratorMap[orderId] = orderIterator;
            }
            else {
                sellMap[orderPrice] = {LimitPrice{orderPrice}};
                idToOrderIteratorMap[orderId] = buyMap[orderPrice].addOrder(order);
            }
            break;
        }
    }

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

int OrderBook::fulfillOrder(Order& orderToFulfill, int numSharesExecuted) {
    int orderShares = orderToFulfill.getShares();
    int sharesToExecute = std::min(orderShares, numSharesExecuted);

    int moneyGenerated = orderToFulfill.execute(sharesToExecute);
    totalVolume += sharesToExecute;

    if(orderToFulfill.getShares() == 0)
        cancelOrder(orderToFulfill.getOrderId());

    return moneyGenerated;
}

int OrderBook::executeOrder(const Order& order) {
    int remainingShares = order.getShares();
    const int orderPrice = order.getLimitPrice();

    int moneyEarned = 0;

    switch(order.getOrderType()) {
        case OrderType::buy: {
            while(remainingShares > 0 && getBestAsk() <= orderPrice) {
                Order& bestAsk = sellMap.begin()->second.getFirstOrder();
                int bestAskShares = bestAsk.getShares();
                int sharesToExecute = std::min(bestAskShares, remainingShares);

                int sellerMoneyGenerated = fulfillOrder(bestAsk, sharesToExecute);
                moneyEarned -= sellerMoneyGenerated;

                remainingShares -= sharesToExecute;
            }
            break;
        }
        case OrderType::sell: {
            while(remainingShares > 0 && getBestBid() >= orderPrice) {
                Order& bestBid = std::prev(buyMap.end())->second.getFirstOrder();
                int bestBidShares = bestBid.getShares();
                int sharesToExecute = std::min(bestBidShares, remainingShares);

                int buyerMoneyGenerated = fulfillOrder(bestBid, sharesToExecute);
                moneyEarned -= buyerMoneyGenerated;

                remainingShares -= sharesToExecute;
            }
            break;
        }
    }

    if(remainingShares > 0) {
        Order updatedOrder{ order.getOrderId(), order.getOrderType(), remainingShares, 
                            order.getLimitPrice(), order.getEntryTime(), order.getTimeInForce()};

        addOrder(updatedOrder);
    }
}

int OrderBook::getVolumeAtLimit(int price) {
    return priceToLimitMap.at(price).getVolume();
}

int OrderBook::getBestBid() {
    return std::prev(buyMap.end())->second.getPrice();
}

int OrderBook::getBestAsk() {
    return sellMap.begin()->second.getPrice();
}

int OrderBook::getTotalVolume() {
    return totalVolume;
}

};