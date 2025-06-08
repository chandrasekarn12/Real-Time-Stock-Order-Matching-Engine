#include "Order.hpp"
#include "MessageQueue.hpp"

#include <random>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <iostream>

void generate_orders(MessageQueue<Order>& queue, 
                     std::atomic<bool>& running,
                     int generator_id,
                     const std::vector<std::string>& tickers,
                     const std::map<std::string, double>& curr_prices,
                     std::mutex& price_mutex) {

    std::default_random_engine engine(std::random_device{}());
    // Have market price drift between -5% and +5% of the current price
    std::uniform_int_distribution<double> drift_dist(-0.05, 0.05);
    // Randomly select a ticker from the list
    std::uniform_int_distribution<int> ticker_dist(0, tickers.size() - 1);
     // 0 for LIMIT, 1 for MARKET
    std::uniform_int_distribution<int> type_dist(0, 1);
    // 0 for BUY, 1 for SELL
    std::uniform_int_distribution<int> side_dist(0, 1);
    // Quantity between 1 and 100
    std::uniform_int_distribution<int> quantity_dist(1, 100);
    // Spread for limit orders between -0.3 and +0.3 of the current price
    std::uniform_real_distribution<double> spread_dist(-0.3, 0.3);
    
    // Local id derivated from generator id for tracking
    uint64_t local_id = generator_id * 1'000'000'000;
    
    while(running) {
        std::string ticker = tickers[ticker_dist(engine)];
        OrderType type = static_cast<OrderType>(type_dist(engine));
        Side side = static_cast<Side>(side_dist(engine));
        uint32_t quantity = quantity_dist(engine);
        double price;
        {
            std::lock_guard<std::mutex> lock(price_mutex);
            
            curr_prices[ticker] += curr_prices[ticker] * drift_dist(engine);
            price = curr_prices[ticker];
        }
        if (type == OrderType::LIMIT) {
            price += price * spread_dist(engine);
        }
        Order order(local_id++, ticker, type, side, price, quantity);
        queue.push(order);
        // Slowing order generation down for now. Will sleep and have different threads
        // sleep for different amounts of time to simulate different traders.
        std::this_thread::sleep_for(std::chrono::milliseconds(200 + generator_id * 50));
    }
}