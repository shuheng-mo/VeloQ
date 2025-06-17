#include "market_data/market_data_feed.h"
#include "utils/logger.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace speedquant {
namespace market_data {

MarketDataFeed::MarketDataFeed(const FeedConfig& config) 
    : config_(config), running_(false) {
    Logger::getInstance().info("MarketDataFeed created with source: {}", config.source);
}

MarketDataFeed::~MarketDataFeed() {
    stop();
    Logger::getInstance().info("MarketDataFeed destroyed");
}

bool MarketDataFeed::start() {
    if (running_) {
        Logger::getInstance().warn("MarketDataFeed already running");
        return true;
    }

    try {
        running_ = true;
        worker_thread_ = std::thread(&MarketDataFeed::run, this);
        Logger::getInstance().info("MarketDataFeed started");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to start MarketDataFeed: {}", e.what());
        running_ = false;
        return false;
    }
}

void MarketDataFeed::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    Logger::getInstance().info("MarketDataFeed stopped");
}

bool MarketDataFeed::subscribe(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (std::find(subscriptions_.begin(), subscriptions_.end(), symbol) != subscriptions_.end()) {
        Logger::getInstance().warn("Already subscribed to symbol: {}", symbol);
        return true;
    }
    
    try {
        // In a real implementation, this would connect to a data provider
        // and subscribe to the symbol
        subscriptions_.push_back(symbol);
        Logger::getInstance().info("Subscribed to symbol: {}", symbol);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to subscribe to symbol {}: {}", symbol, e.what());
        return false;
    }
}

bool MarketDataFeed::unsubscribe(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find(subscriptions_.begin(), subscriptions_.end(), symbol);
    if (it == subscriptions_.end()) {
        Logger::getInstance().warn("Not subscribed to symbol: {}", symbol);
        return true;
    }
    
    try {
        // In a real implementation, this would connect to a data provider
        // and unsubscribe from the symbol
        subscriptions_.erase(it);
        Logger::getInstance().info("Unsubscribed from symbol: {}", symbol);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to unsubscribe from symbol {}: {}", symbol, e.what());
        return false;
    }
}

std::vector<std::string> MarketDataFeed::getSubscriptions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscriptions_;
}

void MarketDataFeed::registerTickCallback(TickCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    tick_callbacks_.push_back(callback);
}

void MarketDataFeed::registerBarCallback(BarCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    bar_callbacks_.push_back(callback);
}

void MarketDataFeed::run() {
    Logger::getInstance().info("MarketDataFeed worker thread started");
    
    while (running_) {
        try {
            // In a real implementation, this would receive data from a provider
            // and process it
            processTicks();
            
            // Sleep for a short time to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.polling_interval_ms));
        } catch (const std::exception& e) {
            Logger::getInstance().error("Error in MarketDataFeed worker thread: {}", e.what());
        }
    }
    
    Logger::getInstance().info("MarketDataFeed worker thread stopped");
}

void MarketDataFeed::processTicks() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Skip if no subscriptions or callbacks
    if (subscriptions_.empty() || tick_callbacks_.empty()) {
        return;
    }
    
    // In a real implementation, this would process actual market data
    // Here we just generate some fake ticks for demonstration
    for (const auto& symbol : subscriptions_) {
        MarketDataTick tick;
        tick.symbol = symbol;
        tick.timestamp = std::chrono::system_clock::now();
        
        // Generate some random price data
        static double last_price = 100.0;
        double price_change = (std::rand() % 100 - 50) / 1000.0;
        last_price += price_change;
        
        tick.price = last_price;
        tick.volume = std::rand() % 1000 + 1;
        tick.bid = last_price - 0.01;
        tick.ask = last_price + 0.01;
        
        // Notify all callbacks
        for (const auto& callback : tick_callbacks_) {
            callback(tick);
        }
    }
    
    // Check if we need to generate a bar
    static auto last_bar_time = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    
    // Generate bars based on the bar interval in the config
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_bar_time).count() >= config_.bar_interval_seconds) {
        for (const auto& symbol : subscriptions_) {
            MarketDataBar bar;
            bar.symbol = symbol;
            bar.timestamp = now;
            bar.interval = config_.bar_interval_seconds;
            
            // Generate some random bar data
            static double last_close = 100.0;
            double price_change = (std::rand() % 100 - 50) / 500.0;
            double close = last_close + price_change;
            
            bar.open = last_close;
            bar.high = std::max(last_close, close) + (std::rand() % 10) / 100.0;
            bar.low = std::min(last_close, close) - (std::rand() % 10) / 100.0;
            bar.close = close;
            bar.volume = std::rand() % 10000 + 1000;
            
            last_close = close;
            
            // Notify all callbacks
            for (const auto& callback : bar_callbacks_) {
                callback(bar);
            }
        }
        
        last_bar_time = now;
    }
}

} // namespace market_data
} // namespace speedquant
