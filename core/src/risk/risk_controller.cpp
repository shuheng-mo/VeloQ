#include "risk/risk_controller.h"
#include "utils/logger.h"

#include <algorithm>
#include <numeric>
#include <cmath>

namespace speedquant {
namespace risk {

RiskController::RiskController(const RiskConfig& config) 
    : config_(config) {
    Logger::getInstance().info("RiskController created");
    
    // Initialize risk rules from config
    for (const auto& rule_config : config.rules) {
        addRule(rule_config);
    }
}

RiskController::~RiskController() {
    Logger::getInstance().info("RiskController destroyed");
}

bool RiskController::addRule(const RiskRuleConfig& rule_config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Create a new rule with the given configuration
        RiskRule rule;
        rule.id = rule_config.id;
        rule.name = rule_config.name;
        rule.type = rule_config.type;
        rule.parameters = rule_config.parameters;
        rule.enabled = rule_config.enabled;
        
        // Add the rule to our list
        rules_.push_back(rule);
        
        Logger::getInstance().info("Added risk rule: ID={}, Name={}, Type={}", 
            rule.id, rule.name, riskRuleTypeToString(rule.type));
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to add risk rule: {}", e.what());
        return false;
    }
}

bool RiskController::removeRule(const std::string& rule_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&rule_id](const RiskRule& rule) { return rule.id == rule_id; });
    
    if (it == rules_.end()) {
        Logger::getInstance().warn("Cannot remove risk rule: Rule ID {} not found", rule_id);
        return false;
    }
    
    try {
        Logger::getInstance().info("Removed risk rule: ID={}, Name={}", 
            it->id, it->name);
        
        rules_.erase(it);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to remove risk rule {}: {}", rule_id, e.what());
        return false;
    }
}

bool RiskController::enableRule(const std::string& rule_id, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&rule_id](const RiskRule& rule) { return rule.id == rule_id; });
    
    if (it == rules_.end()) {
        Logger::getInstance().warn("Cannot update risk rule: Rule ID {} not found", rule_id);
        return false;
    }
    
    try {
        it->enabled = enabled;
        
        Logger::getInstance().info("Risk rule {} {}: ID={}, Name={}", 
            enabled ? "enabled" : "disabled", it->id, it->name);
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to update risk rule {}: {}", rule_id, e.what());
        return false;
    }
}

std::vector<RiskRule> RiskController::getRules() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return rules_;
}

bool RiskController::addPosition(const Position& position) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Check if the position already exists
        auto it = std::find_if(positions_.begin(), positions_.end(),
            [&position](const Position& p) { 
                return p.symbol == position.symbol && p.account == position.account; 
            });
        
        if (it != positions_.end()) {
            // Update existing position
            it->quantity = position.quantity;
            it->avg_price = position.avg_price;
            it->unrealized_pnl = position.unrealized_pnl;
            it->realized_pnl = position.realized_pnl;
            it->last_update_time = position.last_update_time;
            
            Logger::getInstance().info("Updated position: Symbol={}, Account={}, Qty={}, AvgPrice={}", 
                position.symbol, position.account, position.quantity, position.avg_price);
        } else {
            // Add new position
            positions_.push_back(position);
            
            Logger::getInstance().info("Added position: Symbol={}, Account={}, Qty={}, AvgPrice={}", 
                position.symbol, position.account, position.quantity, position.avg_price);
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to add position: {}", e.what());
        return false;
    }
}

bool RiskController::removePosition(const std::string& symbol, const std::string& account) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(positions_.begin(), positions_.end(),
        [&symbol, &account](const Position& p) { 
            return p.symbol == symbol && p.account == account; 
        });
    
    if (it == positions_.end()) {
        Logger::getInstance().warn("Cannot remove position: Position for symbol {} in account {} not found", 
            symbol, account);
        return false;
    }
    
    try {
        Logger::getInstance().info("Removed position: Symbol={}, Account={}", 
            it->symbol, it->account);
        
        positions_.erase(it);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to remove position for symbol {} in account {}: {}", 
            symbol, account, e.what());
        return false;
    }
}

std::vector<Position> RiskController::getPositions(const std::string& account) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (account.empty()) {
        return positions_;
    }
    
    std::vector<Position> result;
    std::copy_if(positions_.begin(), positions_.end(), std::back_inserter(result),
        [&account](const Position& p) { return p.account == account; });
    
    return result;
}

std::optional<Position> RiskController::getPosition(const std::string& symbol, const std::string& account) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(positions_.begin(), positions_.end(),
        [&symbol, &account](const Position& p) { 
            return p.symbol == symbol && p.account == account; 
        });
    
    if (it == positions_.end()) {
        return std::nullopt;
    }
    
    return *it;
}

RiskCheckResult RiskController::checkOrderRisk(const order::Order& order) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    RiskCheckResult result;
    result.passed = true;
    
    // Skip risk check if no rules are defined or enabled
    if (rules_.empty()) {
        return result;
    }
    
    try {
        // Apply each enabled risk rule
        for (const auto& rule : rules_) {
            if (!rule.enabled) {
                continue;
            }
            
            bool rule_passed = true;
            std::string message;
            
            switch (rule.type) {
                case RiskRuleType::MAX_ORDER_SIZE:
                    std::tie(rule_passed, message) = checkMaxOrderSize(order, rule);
                    break;
                    
                case RiskRuleType::MAX_POSITION_SIZE:
                    std::tie(rule_passed, message) = checkMaxPositionSize(order, rule);
                    break;
                    
                case RiskRuleType::MAX_CONCENTRATION:
                    std::tie(rule_passed, message) = checkMaxConcentration(order, rule);
                    break;
                    
                case RiskRuleType::MAX_DRAWDOWN:
                    std::tie(rule_passed, message) = checkMaxDrawdown(order, rule);
                    break;
                    
                case RiskRuleType::CUSTOM:
                    // Custom rules would be implemented by derived classes
                    rule_passed = true;
                    break;
                    
                default:
                    Logger::getInstance().warn("Unknown risk rule type: {}", 
                        static_cast<int>(rule.type));
                    rule_passed = true;
                    break;
            }
            
            if (!rule_passed) {
                result.passed = false;
                result.failed_rules.push_back(rule.id);
                result.messages.push_back(message);
                
                Logger::getInstance().warn("Order failed risk check: Rule={}, Message={}", 
                    rule.name, message);
                
                // If configured to fail fast, return immediately
                if (config_.fail_fast) {
                    break;
                }
            }
        }
        
        return result;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error during risk check: {}", e.what());
        
        result.passed = false;
        result.messages.push_back(std::string("Error during risk check: ") + e.what());
        
        return result;
    }
}

std::pair<bool, std::string> RiskController::checkMaxOrderSize(
    const order::Order& order, const RiskRule& rule) {
    
    // Extract the max order size parameter
    auto it = rule.parameters.find("max_size");
    if (it == rule.parameters.end()) {
        return {true, ""};
    }
    
    double max_size = std::stod(it->second);
    
    // Check if the order size exceeds the maximum
    if (order.quantity > max_size) {
        std::stringstream ss;
        ss << "Order quantity " << order.quantity << " exceeds maximum allowed " << max_size;
        return {false, ss.str()};
    }
    
    return {true, ""};
}

std::pair<bool, std::string> RiskController::checkMaxPositionSize(
    const order::Order& order, const RiskRule& rule) {
    
    // Extract the max position size parameter
    auto it = rule.parameters.find("max_size");
    if (it == rule.parameters.end()) {
        return {true, ""};
    }
    
    double max_size = std::stod(it->second);
    
    // Get the current position for this symbol and account
    auto position_opt = getPosition(order.symbol, order.account);
    double current_position = position_opt ? position_opt->quantity : 0.0;
    
    // Calculate the new position size after this order
    double new_position = current_position;
    if (order.side == order::OrderSide::BUY) {
        new_position += order.quantity;
    } else {
        new_position -= order.quantity;
    }
    
    // Check if the absolute position size exceeds the maximum
    if (std::abs(new_position) > max_size) {
        std::stringstream ss;
        ss << "Resulting position size " << std::abs(new_position) 
           << " would exceed maximum allowed " << max_size;
        return {false, ss.str()};
    }
    
    return {true, ""};
}

std::pair<bool, std::string> RiskController::checkMaxConcentration(
    const order::Order& order, const RiskRule& rule) {
    
    // Extract the max concentration parameter (as a percentage)
    auto it = rule.parameters.find("max_concentration");
    if (it == rule.parameters.end()) {
        return {true, ""};
    }
    
    double max_concentration = std::stod(it->second) / 100.0; // Convert from percentage
    
    // Get the current position for this symbol and account
    auto position_opt = getPosition(order.symbol, order.account);
    double current_position_value = position_opt ? 
        position_opt->quantity * position_opt->avg_price : 0.0;
    
    // Calculate the value of this order
    double order_price = order.price ? *order.price : 
        (position_opt ? position_opt->avg_price : 100.0); // Default price if none specified
    double order_value = order.quantity * order_price;
    
    // Calculate the new position value after this order
    double new_position_value = current_position_value;
    if (order.side == order::OrderSide::BUY) {
        new_position_value += order_value;
    } else {
        new_position_value -= order_value;
    }
    
    // Calculate the total portfolio value
    double total_portfolio_value = 0.0;
    for (const auto& pos : positions_) {
        if (pos.account == order.account) {
            total_portfolio_value += std::abs(pos.quantity * pos.avg_price);
        }
    }
    
    // Add the value of this order to the total
    if (order.side == order::OrderSide::BUY) {
        total_portfolio_value += order_value;
    } else {
        // For sell orders, we're reducing exposure, so don't add to total
    }
    
    // Check if the concentration exceeds the maximum
    if (total_portfolio_value > 0 && 
        std::abs(new_position_value) / total_portfolio_value > max_concentration) {
        std::stringstream ss;
        ss << "Resulting position concentration " 
           << (std::abs(new_position_value) / total_portfolio_value * 100.0) 
           << "% would exceed maximum allowed " << (max_concentration * 100.0) << "%";
        return {false, ss.str()};
    }
    
    return {true, ""};
}

std::pair<bool, std::string> RiskController::checkMaxDrawdown(
    const order::Order& order, const RiskRule& rule) {
    
    // Extract the max drawdown parameter (as a percentage)
    auto it = rule.parameters.find("max_drawdown");
    if (it == rule.parameters.end()) {
        return {true, ""};
    }
    
    double max_drawdown = std::stod(it->second) / 100.0; // Convert from percentage
    
    // In a real implementation, this would check the current drawdown
    // against the maximum allowed drawdown
    
    // For this example, we'll just use a placeholder implementation
    double current_drawdown = 0.05; // 5% drawdown
    
    if (current_drawdown > max_drawdown) {
        std::stringstream ss;
        ss << "Current drawdown " << (current_drawdown * 100.0) 
           << "% exceeds maximum allowed " << (max_drawdown * 100.0) << "%";
        return {false, ss.str()};
    }
    
    return {true, ""};
}

std::string RiskController::riskRuleTypeToString(RiskRuleType type) {
    switch (type) {
        case RiskRuleType::MAX_ORDER_SIZE: return "MAX_ORDER_SIZE";
        case RiskRuleType::MAX_POSITION_SIZE: return "MAX_POSITION_SIZE";
        case RiskRuleType::MAX_CONCENTRATION: return "MAX_CONCENTRATION";
        case RiskRuleType::MAX_DRAWDOWN: return "MAX_DRAWDOWN";
        case RiskRuleType::CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

} // namespace risk
} // namespace speedquant
