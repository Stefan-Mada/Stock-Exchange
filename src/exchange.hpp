/**
 * @file exchange.hpp
 * @author Stefan Mada (me@stefanmada.com)
 * @brief Defines the exchange class which holds numerous orderBooks
 * @version 1.0
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

#include "exchangeOrder.hpp"
#include "orderBook.hpp"
#include <unordered_map>

namespace Exchange {

struct Exchange {
    Exchange() = default;

    void addOrder(const ExchangeOrder& order);

private:
    std::unordered_map<std::string, OrderBook> orderBookMap;
};

} // namespace Exchange
#endif