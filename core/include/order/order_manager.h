#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <mutex>
#include <functional>

namespace speedquant {
namespace order {

enum class OrderType {
    MARKET,
    LIMIT,
    STOP,
    STOP_LIMIT
};

enum class OrderSide {
    BUY,
    SELL
};

enum class OrderStatus {
    PENDING,
    SUBMITTED,
    PARTIAL_FILLED,
    FILLED,
    CANCELED,
    REJECTED
};

struct Order {
    std::string order_id;
    std::string symbol;
    OrderType type;
    OrderSide side;
    OrderStatus status;
    double price;
    double stop_price;  // For stop orders
    double quantity;
    double filled_quantity;
    double avg_fill_price;
    int64_t create_time;
    int64_t update_time;
    std::string account;
    std::string strategy_id;
    std::map<std::string, std::string> extra_params;
};

struct OrderConfig {
    std::string exchange;
    std::string api_key;
    std::string api_secret;
    std::string account;
    bool simulation_mode = false;
};

class OrderManager {
public:
    explicit OrderManager(const OrderConfig& config);
    ~OrderManager();

    // Non-copyable
    OrderManager(const OrderManager&) = delete;
    OrderManager& operator=(const OrderManager&) = delete;

    // Order operations
    std::string submit_order(const Order& order);
    bool cancel_order(const std::string& order_id);
    bool cancel_all_orders(const std::string& symbol = "");
    
    // Order status
    Order get_order(const std::string& order_id);
    std::vector<Order> get_open_orders(const std::string& symbol = "");
    
    // Callbacks
    using OrderUpdateCallback = std::function<void(const Order&)>;
    void register_order_update_callback(OrderUpdateCallback callback);
    
private:
    OrderConfig config_;
    std::mutex mutex_;
    std::map<std::string, Order> orders_;
    std::vector<OrderUpdateCallback> callbacks_;
    
    // Internal methods
    void process_order_update(const Order& order);
};

} // namespace order
} // namespace speedquant
