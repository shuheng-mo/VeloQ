#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace veloq {
namespace common {

// Timestamp type (microseconds since epoch)
using Timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

// Price type (in ticks, to avoid floating-point precision issues)
using Price = int64_t;

// Volume type
using Volume = int64_t;

// Order ID type
using OrderId = uint64_t;

// Instrument/Symbol identifier
using InstrumentId = std::string;

// Side of order/trade
enum class Side : uint8_t {
    BUY = 0,
    SELL = 1,
    UNKNOWN = 255
};

// Market data tick structure
struct MarketTick {
    InstrumentId instrument_id;
    Timestamp timestamp;

    Price bid_price[5];    // Top 5 bid prices
    Volume bid_volume[5];  // Top 5 bid volumes
    Price ask_price[5];    // Top 5 ask prices
    Volume ask_volume[5];  // Top 5 ask volumes

    Price last_price;
    Volume last_volume;
    Volume total_volume;

    MarketTick() = default;
};

} // namespace common
} // namespace veloq
