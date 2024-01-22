/**
 * @file exchange.cpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Implements the exchange class
 * @version 1.0
 * @date 2024-01-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "exchange.hpp"
#include <stdexcept>

namespace Exchange {

bool Exchange::canPlaceOrder(int userId, const ExchangeOrder& order) {
    const auto& user = userMap.at(userId);

    if(order.orderAction == OrderAction::buy && user.getCashTradeable() < order.price * order.shares) {
        return false;
    }
    else if (order.orderAction == OrderAction::sell && user.getSharesOwned(order.symbol) < order.shares) {
        return false;
    }

    return true;
}

void Exchange::addOrder(int userId, const ExchangeOrder& order) {
    // TODO: Have this return meaningful errors up the chain
    if(!orderBookMap.contains(order.symbol) || userMap.contains(userId))
        return;
    if(!canPlaceOrder(userId, order))
        return;

    // TODO: This is currently only working for 'gtc' TimeInForce, fix that
    if(order.timeInForce != TimeInForce::gtc)
        throw std::invalid_argument("Exchange::addOrder only supports gtc time in force currently");

    // TODO: Also only currently working for limit OrderType, fix that
    if(order.orderType != OrderType::limit)
        throw std::invalid_argument("Exchange::addOrder only supports limit OrderType currently");

    
    OrderExecution orderExec = orderBookMap.at(order.symbol).addOrder(order.orderAction, order.shares, order.price, 0);

    // need to parse orderExec object to update Users
    // User that placed order is no 1
    // Need to handle case of partial execution of user's placed order

    // Users that had full executions is 2
    // Optionally, user that had partial execution is 3
}

} // namespace Exchange