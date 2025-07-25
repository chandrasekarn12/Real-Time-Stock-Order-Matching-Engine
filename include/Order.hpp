#pragma once
#include <string>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <sstream>

enum class OrderType {LIMIT, MARKET};
// Limit orders are executed at a specific price or better
// Market orders are executed at the best current price available
enum class Side {BUY, SELL};

struct Order {
    uint64_t id; //unique order ID, use 64-bit unsigned to prevent overflow
    std::string ticker;
    OrderType type;
    Side side;
    double price; //only used for LIMIT orders
    uint32_t quantity; //Only 32-bits needed, can be lowered if necessary
    std::chrono::high_resolution_clock::time_point timestamp;

    Order(uint64_t id, std::string ticker, OrderType type, Side side, double price, uint32_t quantity)
        : ticker(ticker), id(id), type(type), side(side), price(price), quantity(quantity), 
          timestamp(std::chrono::high_resolution_clock::now()) {}
    
    std::string to_string() const {
        std::ostringstream oss;
        oss << "Order [ID: " << id 
            << "; Type: " << (type == OrderType::LIMIT ? "LIMIT" : "MARKET")
            << "; Side: " << (side == Side::BUY ? "BUY" : "SELL")
            << "; Price: " << price
            << "; Quantity: " << quantity
            << "; Timestamp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
            oss << "]";
        return oss.str();
    }
};