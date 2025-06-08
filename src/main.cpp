#include "../include/Order.hpp"
#include "../include/MessageQueue.hpp"
#include "OrderGenerator.cpp"

#include <thread>
#include <vector>
#include <map>
#include <atomic>
#include <iostream>

int main() {
    MessageQueue<Order> order_queue;
    std::atomic<bool> running(true);

    std::vector<std::string> tickers = {"META", "AMZN", "AAPL", "NFLX", "GOOGL"};
    std::map<std::string, double> curr_prices = {
        {"META", 697.71}, {"AMZN", 213.57}, {"AAPL", 203.92}, 
        {"NFLX", 1241.48}, {"GOOGL", 174.92}
    };
    std::mutex price_mutex;
    std::vector<std::thread> generators;

    for (int i = 0; i < 5; ++i) {
        generators.emplace_back(generate_orders, std::ref(order_queue),
                                std::ref(running), 
                                i, 
                                std::ref(tickers), 
                                std::ref(curr_prices), 
                                std::ref(price_mutex));
    }

    // Run for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));
    running = false;

    for (auto& gen : generators) {
        if (gen.joinable()) {
            gen.join();
        }
    }

    while (!order_queue.empty()) {
        Order order = order_queue.pop();
        std::cout << "Processed Order: " << order.to_string() << std::endl;
    }
}