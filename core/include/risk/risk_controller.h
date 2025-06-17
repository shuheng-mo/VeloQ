#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <mutex>

#include "../order/order_manager.h"

namespace speedquant {
namespace risk {

struct Position {
    std::string symbol;
    double quantity;
    double avg_price;
    double unrealized_pnl;
    double realized_pnl;
    std::string account;
    std::string strategy_id;
};

struct RiskRule {
    std::string id;
    std::string name;
    std::string description;
    std::function<bool(const order::Order&, const std::map<std::string, Position>&)> check_order;
    bool enabled = true;
};

struct RiskConfig {
    double max_position_size = 0.0;  // 0 means no limit
    double max_order_value = 0.0;    // 0 means no limit
    double max_drawdown = 0.0;       // 0 means no limit
    double daily_loss_limit = 0.0;   // 0 means no limit
    std::map<std::string, double> symbol_position_limits;
    bool enable_stop_loss = true;
    bool enable_take_profit = true;
};

class RiskController {
public:
    explicit RiskController(const RiskConfig& config);
    ~RiskController();

    // Non-copyable
    RiskController(const RiskController&) = delete;
    RiskController& operator=(const RiskController&) = delete;

    // Risk check
    bool check_order(const order::Order& order);
    
    // Position management
    void update_position(const Position& position);
    Position get_position(const std::string& symbol, const std::string& account = "", const std::string& strategy_id = "");
    std::vector<Position> get_all_positions();
    
    // Risk rules management
    void add_risk_rule(const RiskRule& rule);
    void remove_risk_rule(const std::string& rule_id);
    void enable_risk_rule(const std::string& rule_id, bool enabled);
    std::vector<RiskRule> get_all_risk_rules();
    
    // Risk metrics
    double get_total_exposure();
    double get_daily_pnl();
    double get_max_drawdown();
    
private:
    RiskConfig config_;
    std::mutex mutex_;
    std::map<std::string, Position> positions_;  // key: symbol-account-strategy_id
    std::map<std::string, RiskRule> risk_rules_;
    
    // Helper methods
    std::string get_position_key(const std::string& symbol, const std::string& account, const std::string& strategy_id);
};

} // namespace risk
} // namespace speedquant
