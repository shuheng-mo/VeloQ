#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace speedquant {
namespace market_data {

struct MarketDataConfig {
    std::string source;
    std::string connection_string;
    int buffer_size = 10000;
    bool use_shared_memory = true;
};

struct Tick {
    std::string symbol;
    double price;
    double volume;
    int64_t timestamp;
    // Additional fields can be added as needed
};

class MarketDataFeed {
public:
    explicit MarketDataFeed(const MarketDataConfig& config);
    ~MarketDataFeed();

    // Non-copyable
    MarketDataFeed(const MarketDataFeed&) = delete;
    MarketDataFeed& operator=(const MarketDataFeed&) = delete;

    // Start/stop the feed
    bool start();
    void stop();
    bool is_running() const;

    // Subscribe to symbols
    bool subscribe(const std::vector<std::string>& symbols);
    bool unsubscribe(const std::vector<std::string>& symbols);

    // Register callback for tick data
    using TickCallback = std::function<void(const Tick&)>;
    void register_callback(TickCallback callback);

private:
    MarketDataConfig config_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::string> subscribed_symbols_;
    std::vector<TickCallback> callbacks_;

    // Worker thread function
    void worker_function();
};

} // namespace market_data
} // namespace speedquant
