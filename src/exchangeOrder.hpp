/**
 * @file exchangeOrder.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief File defines the order received by the Exchange class with detailed order information
 * @version 1.0
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef EXCHANGEORDER_HPP
#define EXCHANGEORDER_HPP

#include "order.hpp"
#include <string>

namespace Exchange {

/**
 * @brief Type of order, either market or limit
 * 
 */
enum OrderType {
    market,
    limit
};

/**
 * @brief Time in force of order
 * 
 */
enum TimeInForce {
    /// @brief For the day
    day, 
    /// @brief Never cancels without user input
    gtc,
    /// @brief 'Fill or kill' - Must fill entirely immediately, or whole order is cancelled
    fok,
    /// @brief 'Immediate or Cancel' - Fills as much as possible, then cancels
    ioc
};

struct ExchangeOrder {
    // Want exchange to be able to read the members without setting useless getters
    friend class Exchange;

    ExchangeOrder(OrderAction orderAction, OrderType orderType, TimeInForce timeInForce, int price, int shares, const std::string& symbol)
                    : orderAction{orderAction}, orderType{orderType}, timeInForce{timeInForce}, price{price}, shares{shares}, symbol{symbol} {}

private:
    const OrderAction orderAction;
    const OrderType orderType;
    const TimeInForce timeInForce;
    const int price;
    const int shares;
    const std::string symbol;
};

} // namespace Exchange

#endif