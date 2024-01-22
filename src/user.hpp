/**
 * @file User.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Defines user class, storing info about each user
 * @version 1.0
 * @date 2024-01-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef USER_HPP
#define USER_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include "order.hpp"

namespace Exchange {

struct User {
    User() = default;

    

    int getCashDeposited() const;
    int getCashTradeable() const;
    const Order& getOpenOrders() const;
    int getSharesOwned(const std::string& symbol) const;
private:
    int cashDeposited = 0;
    int cashTradeable = 0;
    std::vector<const Order&> openOrders; 
    std::unordered_map<std::string, int> sharesOwned;
};

} // namespace Exchange

#endif