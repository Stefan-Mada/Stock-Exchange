/**
 * @file order.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Header file for the struct representing a single limit order
 * @version 1.0
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ORDER_HPP
#define ORDER_HPP

namespace Exchange {

/**
 * @brief Order types possible for an order: currently buy or sell
 * 
 */
enum OrderType {
    buy,
    sell
};

/**
 * @brief The Order struct, which holds all necessary information for a single order
 * 
 */
struct Order {
    Order(int orderId, OrderType orderType, int shares, int limitPrice, int entryTime, int timeInForce)
        : orderId{orderId}, orderType{orderType}, shares{shares}, limitPrice{limitPrice}, entryTime{entryTime}, timeInForce{timeInForce} {}

    OrderType getOrderType() const;
    int getLimitPrice() const;
    int getOrderId() const;
    int getShares() const;
    int getEntryTime() const;
    int getTimeInForce() const;

    /**
     * @brief Fills some or all of the shares of this order, but not all. Modifies this order object.
     * 
     * @param numShares Number of shares executed
     * @return Amount of money this order has earned
     */
    int execute(int numShares);

private:
    int orderId;
    OrderType orderType;
    int shares;
    int limitPrice;
    int entryTime;
    int timeInForce;
    int moneyEarned = 0;
};

};

#endif