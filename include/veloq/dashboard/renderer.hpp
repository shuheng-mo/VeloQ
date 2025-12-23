#pragma once

#include "veloq/common/types.hpp"
#include "veloq/feature_engine/features.hpp"
#include "veloq/inference/model.hpp"
#include <string>
#include <vector>

namespace veloq {
namespace dashboard {

/**
 * @brief Dashboard data for visualization
 */
struct DashboardData {
    // Current market tick
    common::MarketTick current_tick;

    // Real-time features
    feature_engine::MarketFeatures features;

    // AI predictions
    inference::Prediction prediction;

    // System performance metrics
    struct {
        int64_t avg_latency_us;
        int64_t max_latency_us;
        uint64_t tick_count;
        double tick_rate;  // ticks per second
    } metrics;
};

/**
 * @brief Dashboard Renderer using Dear ImGui
 *
 * Provides real-time visualization of:
 * - Order book heatmap
 * - AI prediction curves
 * - System latency monitoring
 * - Feature values timeline
 */
class DashboardRenderer {
public:
    DashboardRenderer();
    ~DashboardRenderer();

    /**
     * @brief Initialize dashboard window
     * @param title Window title
     * @param width Window width
     * @param height Window height
     * @return true if initialization successful
     */
    bool initialize(const std::string& title = "VeloQ Dashboard",
                   int width = 1920,
                   int height = 1080);

    /**
     * @brief Update dashboard with new data
     * @param data Dashboard data to display
     */
    void update(const DashboardData& data);

    /**
     * @brief Render one frame
     * @return true if should continue, false if window closed
     */
    bool render();

    /**
     * @brief Cleanup resources
     */
    void shutdown();

private:
    bool initialized_;
    std::vector<DashboardData> history_;  // Historical data for charts

    // ImGui rendering functions
    void render_orderbook(const common::MarketTick& tick);
    void render_features(const feature_engine::MarketFeatures& features);
    void render_predictions(const inference::Prediction& prediction);
    void render_metrics(const DashboardData::metrics& metrics);
};

} // namespace dashboard
} // namespace veloq
