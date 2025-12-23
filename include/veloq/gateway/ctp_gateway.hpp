#pragma once

#include "veloq/common/types.hpp"
#include "veloq/common/lockfree_queue.hpp"
#include <string>
#include <functional>

namespace veloq {
namespace gateway {

/**
 * @brief CTP Market Data Gateway
 *
 * Encapsulates CTP API for ultra-low latency market data reception.
 * Uses lock-free queue for decoupling network I/O from data processing.
 */
class CtpGateway {
public:
    using TickCallback = std::function<void(const common::MarketTick&)>;

    CtpGateway();
    ~CtpGateway();

    /**
     * @brief Connect to CTP server
     * @param front_addr CTP front server address
     * @param broker_id Broker ID
     * @param user_id User ID
     * @param password Password
     * @return true if connection successful
     */
    bool connect(const std::string& front_addr,
                 const std::string& broker_id,
                 const std::string& user_id,
                 const std::string& password);

    /**
     * @brief Subscribe to market data for instruments
     * @param instruments List of instrument IDs
     * @return true if subscription successful
     */
    bool subscribe(const std::vector<std::string>& instruments);

    /**
     * @brief Start receiving market data
     * @param callback Callback function for received ticks
     */
    void start(TickCallback callback);

    /**
     * @brief Stop receiving market data
     */
    void stop();

    /**
     * @brief Check if gateway is connected
     */
    bool is_connected() const { return connected_; }

private:
    bool connected_;
    common::LockFreeQueue<common::MarketTick> tick_queue_;
    // CTP API objects will be added here
};

} // namespace gateway
} // namespace veloq
