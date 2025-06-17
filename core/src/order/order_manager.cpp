#include "order/order_manager.h"
#include "utils/logger.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>

namespace speedquant {
namespace order {

OrderManager::OrderManager(const OrderManagerConfig& config) 
    : config_(config), running_(false), next_order_id_(1) {
    Logger::getInstance().info("OrderManager created with broker: {}", config.broker);
}

OrderManager::~OrderManager() {
    stop();
    Logger::getInstance().info("OrderManager destroyed");
}

bool OrderManager::start() {
    if (running_) {
        Logger::getInstance().warn("OrderManager already running");
        return true;
    }

    try {
        running_ = true;
        worker_thread_ = std::thread(&OrderManager::run, this);
        Logger::getInstance().info("OrderManager started");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to start OrderManager: {}", e.what());
        running_ = false;
        return false;
    }
}

void OrderManager::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    Logger::getInstance().info("OrderManager stopped");
}

std::string OrderManager::submitOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Generate a unique order ID
    std::string order_id = generateOrderId();
    
    try {
        // Create a copy of the order with the assigned ID
        Order new_order = order;
        new_order.order_id = order_id;
        new_order.status = OrderStatus::PENDING;
        new_order.create_time = std::chrono::system_clock::now();
        new_order.update_time = new_order.create_time;
        
        // Store the order
        orders_[order_id] = new_order;
        
        Logger::getInstance().info("Order submitted: ID={}, Symbol={}, Type={}, Side={}, Qty={}, Price={}",
            order_id, order.symbol, orderTypeToString(order.type), 
            orderSideToString(order.side), order.quantity, 
            order.price ? std::to_string(*order.price) : "N/A");
        
        // Notify callbacks
        for (const auto& callback : order_callbacks_) {
            callback(new_order);
        }
        
        return order_id;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to submit order: {}", e.what());
        return "";
    }
}

bool OrderManager::cancelOrder(const std::string& order_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = orders_.find(order_id);
    if (it == orders_.end()) {
        Logger::getInstance().warn("Cannot cancel order: Order ID {} not found", order_id);
        return false;
    }
    
    // Check if the order can be canceled
    if (it->second.status == OrderStatus::FILLED || 
        it->second.status == OrderStatus::CANCELED ||
        it->second.status == OrderStatus::REJECTED) {
        Logger::getInstance().warn("Cannot cancel order: Order ID {} is already in state {}", 
            order_id, orderStatusToString(it->second.status));
        return false;
    }
    
    try {
        // Update the order status
        it->second.status = OrderStatus::CANCELED;
        it->second.update_time = std::chrono::system_clock::now();
        
        Logger::getInstance().info("Order canceled: ID={}", order_id);
        
        // Notify callbacks
        for (const auto& callback : order_callbacks_) {
            callback(it->second);
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to cancel order {}: {}", order_id, e.what());
        return false;
    }
}

std::optional<Order> OrderManager::getOrder(const std::string& order_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = orders_.find(order_id);
    if (it == orders_.end()) {
        return std::nullopt;
    }
    
    return it->second;
}

std::vector<Order> OrderManager::getOrders(const OrderFilter& filter) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<Order> result;
    
    for (const auto& [order_id, order] : orders_) {
        bool match = true;
        
        // Apply filters
        if (filter.symbol && order.symbol != *filter.symbol) {
            match = false;
        }
        
        if (filter.status && order.status != *filter.status) {
            match = false;
        }
        
        if (filter.side && order.side != *filter.side) {
            match = false;
        }
        
        if (filter.type && order.type != *filter.type) {
            match = false;
        }
        
        if (match) {
            result.push_back(order);
        }
    }
    
    return result;
}

void OrderManager::registerOrderCallback(OrderCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    order_callbacks_.push_back(callback);
}

void OrderManager::registerTradeCallback(TradeCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    trade_callbacks_.push_back(callback);
}

void OrderManager::run() {
    Logger::getInstance().info("OrderManager worker thread started");
    
    while (running_) {
        try {
            // In a real implementation, this would communicate with a broker
            // and process order updates
            processOrders();
            
            // Sleep for a short time to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.polling_interval_ms));
        } catch (const std::exception& e) {
            Logger::getInstance().error("Error in OrderManager worker thread: {}", e.what());
        }
    }
    
    Logger::getInstance().info("OrderManager worker thread stopped");
}

void OrderManager::processOrders() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Process each pending or submitted order
    for (auto& [order_id, order] : orders_) {
        if (order.status == OrderStatus::PENDING) {
            // Simulate order submission to broker
            order.status = OrderStatus::SUBMITTED;
            order.update_time = std::chrono::system_clock::now();
            
            Logger::getInstance().info("Order submitted to broker: ID={}", order_id);
            
            // Notify callbacks
            for (const auto& callback : order_callbacks_) {
                callback(order);
            }
        } else if (order.status == OrderStatus::SUBMITTED || order.status == OrderStatus::PARTIAL_FILLED) {
            // Simulate order execution
            // In a real implementation, this would receive updates from the broker
            
            // Randomly decide if the order gets filled
            if (std::rand() % 10 < 3) { // 30% chance of fill per cycle
                double fill_quantity = 0.0;
                
                if (order.status == OrderStatus::SUBMITTED) {
                    // For simplicity, fill the entire order or a random partial amount
                    fill_quantity = (std::rand() % 10 < 7) ? 
                        order.quantity : // 70% chance of full fill
                        order.quantity * (0.1 + 0.8 * (std::rand() % 10) / 10.0); // 10-90% partial fill
                } else {
                    // Already partially filled, fill the remaining amount or a portion of it
                    double remaining = order.quantity - order.filled_quantity;
                    fill_quantity = (std::rand() % 10 < 7) ? 
                        remaining : // 70% chance of full remaining fill
                        remaining * (0.1 + 0.8 * (std::rand() % 10) / 10.0); // 10-90% partial fill
                }
                
                // Round to 2 decimal places
                fill_quantity = std::round(fill_quantity * 100.0) / 100.0;
                
                // Ensure we don't exceed the order quantity
                fill_quantity = std::min(fill_quantity, order.quantity - order.filled_quantity);
                
                if (fill_quantity > 0) {
                    // Create a trade
                    Trade trade;
                    trade.trade_id = generateTradeId();
                    trade.order_id = order_id;
                    trade.symbol = order.symbol;
                    trade.side = order.side;
                    trade.quantity = fill_quantity;
                    
                    // Calculate fill price with some slippage
                    double slippage = (std::rand() % 100 - 50) / 10000.0; // -0.5% to +0.5%
                    
                    if (order.type == OrderType::MARKET) {
                        // For market orders, use a price with slippage
                        trade.price = order.price ? 
                            *order.price * (1.0 + slippage) : 
                            100.0 * (1.0 + slippage); // Default price if none specified
                    } else if (order.type == OrderType::LIMIT) {
                        // For limit orders, ensure the price is not worse than the limit
                        if (order.side == OrderSide::BUY) {
                            trade.price = order.price ? 
                                std::min(*order.price, *order.price * (1.0 + slippage)) : 
                                100.0;
                        } else {
                            trade.price = order.price ? 
                                std::max(*order.price, *order.price * (1.0 + slippage)) : 
                                100.0;
                        }
                    } else {
                        // For other order types, use the specified price or a default
                        trade.price = order.price ? *order.price : 100.0;
                    }
                    
                    trade.timestamp = std::chrono::system_clock::now();
                    
                    // Update the order
                    order.filled_quantity += fill_quantity;
                    
                    // Calculate average fill price
                    if (!order.avg_fill_price) {
                        order.avg_fill_price = trade.price;
                    } else {
                        double total_quantity = order.filled_quantity;
                        double prev_qty = total_quantity - fill_quantity;
                        order.avg_fill_price = ((*order.avg_fill_price * prev_qty) + (trade.price * fill_quantity)) / total_quantity;
                    }
                    
                    // Update order status
                    if (std::abs(order.filled_quantity - order.quantity) < 0.000001) {
                        order.status = OrderStatus::FILLED;
                    } else {
                        order.status = OrderStatus::PARTIAL_FILLED;
                    }
                    
                    order.update_time = std::chrono::system_clock::now();
                    
                    Logger::getInstance().info("Order filled: ID={}, Fill Qty={}, Fill Price={}, Total Filled={}, Status={}",
                        order_id, fill_quantity, trade.price, order.filled_quantity, orderStatusToString(order.status));
                    
                    // Notify callbacks
                    for (const auto& callback : order_callbacks_) {
                        callback(order);
                    }
                    
                    for (const auto& callback : trade_callbacks_) {
                        callback(trade);
                    }
                }
            }
        }
    }
}

std::string OrderManager::generateOrderId() {
    // Generate a unique order ID
    std::stringstream ss;
    ss << "ORD-" << std::this_thread::get_id() << "-" << next_order_id_++;
    return ss.str();
}

std::string OrderManager::generateTradeId() {
    // Generate a unique trade ID
    static int next_trade_id = 1;
    std::stringstream ss;
    ss << "TRD-" << std::this_thread::get_id() << "-" << next_trade_id++;
    return ss.str();
}

std::string OrderManager::orderTypeToString(OrderType type) {
    switch (type) {
        case OrderType::MARKET: return "MARKET";
        case OrderType::LIMIT: return "LIMIT";
        case OrderType::STOP: return "STOP";
        case OrderType::STOP_LIMIT: return "STOP_LIMIT";
        default: return "UNKNOWN";
    }
}

std::string OrderManager::orderSideToString(OrderSide side) {
    switch (side) {
        case OrderSide::BUY: return "BUY";
        case OrderSide::SELL: return "SELL";
        default: return "UNKNOWN";
    }
}

std::string OrderManager::orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING: return "PENDING";
        case OrderStatus::SUBMITTED: return "SUBMITTED";
        case OrderStatus::PARTIAL_FILLED: return "PARTIAL_FILLED";
        case OrderStatus::FILLED: return "FILLED";
        case OrderStatus::CANCELED: return "CANCELED";
        case OrderStatus::REJECTED: return "REJECTED";
        default: return "UNKNOWN";
    }
}

} // namespace order
} // namespace speedquant
