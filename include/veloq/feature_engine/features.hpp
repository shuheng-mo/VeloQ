#pragma once

#include "veloq/common/types.hpp"
#include <array>

namespace veloq {
namespace feature_engine {

/**
 * @brief Computed market microstructure features
 */
struct MarketFeatures {
    // Order Flow Imbalance (OFI)
    double ofi;

    // Book pressure (bid vs ask imbalance)
    double book_pressure;

    // Bid-ask spread
    double spread;

    // Volume-weighted average price
    double vwap;

    // Mid price
    double mid_price;

    // Timestamp of feature calculation
    common::Timestamp timestamp;
};

/**
 * @brief Feature Engine for real-time feature computation
 *
 * Computes market microstructure features with sub-millisecond latency.
 * Optimized with SIMD instructions and cache-friendly memory layout.
 */
class FeatureEngine {
public:
    FeatureEngine();
    ~FeatureEngine();

    /**
     * @brief Compute features from market tick
     * @param tick Input market tick data
     * @return Computed features
     */
    MarketFeatures compute(const common::MarketTick& tick);

    /**
     * @brief Reset feature engine state
     */
    void reset();

private:
    // Previous tick for OFI calculation
    common::MarketTick prev_tick_;

    // Rolling window for VWAP calculation
    static constexpr size_t WINDOW_SIZE = 100;
    std::array<common::Price, WINDOW_SIZE> price_window_;
    std::array<common::Volume, WINDOW_SIZE> volume_window_;
    size_t window_index_;
};

} // namespace feature_engine
} // namespace veloq
