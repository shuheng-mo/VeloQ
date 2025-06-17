#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "market_data/market_data_feed.h"
#include "order/order_manager.h"
#include "risk/risk_controller.h"
#include "utils/logger.h"
#include "utils/config_manager.h"

namespace py = pybind11;

PYBIND11_MODULE(speedquant_core, m) {
    m.doc() = "SpeedQuant C++ core bindings";
    
    // Version info
    m.attr("__version__") = "0.1.0";
    
    // Market data module
    py::module market_data = m.def_submodule("market_data", "Market data module");
    
    py::class_<speedquant::market_data::MarketDataConfig>(market_data, "MarketDataConfig")
        .def(py::init<>())
        .def_readwrite("source", &speedquant::market_data::MarketDataConfig::source)
        .def_readwrite("connection_string", &speedquant::market_data::MarketDataConfig::connection_string)
        .def_readwrite("buffer_size", &speedquant::market_data::MarketDataConfig::buffer_size)
        .def_readwrite("use_shared_memory", &speedquant::market_data::MarketDataConfig::use_shared_memory);
    
    py::class_<speedquant::market_data::Tick>(market_data, "Tick")
        .def(py::init<>())
        .def_readwrite("symbol", &speedquant::market_data::Tick::symbol)
        .def_readwrite("price", &speedquant::market_data::Tick::price)
        .def_readwrite("volume", &speedquant::market_data::Tick::volume)
        .def_readwrite("timestamp", &speedquant::market_data::Tick::timestamp);
    
    py::class_<speedquant::market_data::MarketDataFeed>(market_data, "MarketDataFeed")
        .def(py::init<const speedquant::market_data::MarketDataConfig&>())
        .def("start", &speedquant::market_data::MarketDataFeed::start)
        .def("stop", &speedquant::market_data::MarketDataFeed::stop)
        .def("is_running", &speedquant::market_data::MarketDataFeed::is_running)
        .def("subscribe", &speedquant::market_data::MarketDataFeed::subscribe)
        .def("unsubscribe", &speedquant::market_data::MarketDataFeed::unsubscribe)
        .def("register_callback", &speedquant::market_data::MarketDataFeed::register_callback);
    
    // Order module
    py::module order = m.def_submodule("order", "Order management module");
    
    py::enum_<speedquant::order::OrderType>(order, "OrderType")
        .value("MARKET", speedquant::order::OrderType::MARKET)
        .value("LIMIT", speedquant::order::OrderType::LIMIT)
        .value("STOP", speedquant::order::OrderType::STOP)
        .value("STOP_LIMIT", speedquant::order::OrderType::STOP_LIMIT)
        .export_values();
    
    py::enum_<speedquant::order::OrderSide>(order, "OrderSide")
        .value("BUY", speedquant::order::OrderSide::BUY)
        .value("SELL", speedquant::order::OrderSide::SELL)
        .export_values();
    
    py::enum_<speedquant::order::OrderStatus>(order, "OrderStatus")
        .value("PENDING", speedquant::order::OrderStatus::PENDING)
        .value("SUBMITTED", speedquant::order::OrderStatus::SUBMITTED)
        .value("PARTIAL_FILLED", speedquant::order::OrderStatus::PARTIAL_FILLED)
        .value("FILLED", speedquant::order::OrderStatus::FILLED)
        .value("CANCELED", speedquant::order::OrderStatus::CANCELED)
        .value("REJECTED", speedquant::order::OrderStatus::REJECTED)
        .export_values();
    
    py::class_<speedquant::order::Order>(order, "Order")
        .def(py::init<>())
        .def_readwrite("order_id", &speedquant::order::Order::order_id)
        .def_readwrite("symbol", &speedquant::order::Order::symbol)
        .def_readwrite("type", &speedquant::order::Order::type)
        .def_readwrite("side", &speedquant::order::Order::side)
        .def_readwrite("status", &speedquant::order::Order::status)
        .def_readwrite("price", &speedquant::order::Order::price)
        .def_readwrite("stop_price", &speedquant::order::Order::stop_price)
        .def_readwrite("quantity", &speedquant::order::Order::quantity)
        .def_readwrite("filled_quantity", &speedquant::order::Order::filled_quantity)
        .def_readwrite("avg_fill_price", &speedquant::order::Order::avg_fill_price)
        .def_readwrite("create_time", &speedquant::order::Order::create_time)
        .def_readwrite("update_time", &speedquant::order::Order::update_time)
        .def_readwrite("account", &speedquant::order::Order::account)
        .def_readwrite("strategy_id", &speedquant::order::Order::strategy_id)
        .def_readwrite("extra_params", &speedquant::order::Order::extra_params);
    
    py::class_<speedquant::order::OrderConfig>(order, "OrderConfig")
        .def(py::init<>())
        .def_readwrite("exchange", &speedquant::order::OrderConfig::exchange)
        .def_readwrite("api_key", &speedquant::order::OrderConfig::api_key)
        .def_readwrite("api_secret", &speedquant::order::OrderConfig::api_secret)
        .def_readwrite("account", &speedquant::order::OrderConfig::account)
        .def_readwrite("simulation_mode", &speedquant::order::OrderConfig::simulation_mode);
    
    py::class_<speedquant::order::OrderManager>(order, "OrderManager")
        .def(py::init<const speedquant::order::OrderConfig&>())
        .def("submit_order", &speedquant::order::OrderManager::submit_order)
        .def("cancel_order", &speedquant::order::OrderManager::cancel_order)
        .def("cancel_all_orders", &speedquant::order::OrderManager::cancel_all_orders)
        .def("get_order", &speedquant::order::OrderManager::get_order)
        .def("get_open_orders", &speedquant::order::OrderManager::get_open_orders)
        .def("register_order_update_callback", &speedquant::order::OrderManager::register_order_update_callback);
    
    // Risk module
    py::module risk = m.def_submodule("risk", "Risk management module");
    
    py::class_<speedquant::risk::Position>(risk, "Position")
        .def(py::init<>())
        .def_readwrite("symbol", &speedquant::risk::Position::symbol)
        .def_readwrite("quantity", &speedquant::risk::Position::quantity)
        .def_readwrite("avg_price", &speedquant::risk::Position::avg_price)
        .def_readwrite("unrealized_pnl", &speedquant::risk::Position::unrealized_pnl)
        .def_readwrite("realized_pnl", &speedquant::risk::Position::realized_pnl)
        .def_readwrite("account", &speedquant::risk::Position::account)
        .def_readwrite("strategy_id", &speedquant::risk::Position::strategy_id);
    
    py::class_<speedquant::risk::RiskRule>(risk, "RiskRule")
        .def(py::init<>())
        .def_readwrite("id", &speedquant::risk::RiskRule::id)
        .def_readwrite("name", &speedquant::risk::RiskRule::name)
        .def_readwrite("description", &speedquant::risk::RiskRule::description)
        .def_readwrite("check_order", &speedquant::risk::RiskRule::check_order)
        .def_readwrite("enabled", &speedquant::risk::RiskRule::enabled);
    
    py::class_<speedquant::risk::RiskConfig>(risk, "RiskConfig")
        .def(py::init<>())
        .def_readwrite("max_position_size", &speedquant::risk::RiskConfig::max_position_size)
        .def_readwrite("max_order_value", &speedquant::risk::RiskConfig::max_order_value)
        .def_readwrite("max_drawdown", &speedquant::risk::RiskConfig::max_drawdown)
        .def_readwrite("daily_loss_limit", &speedquant::risk::RiskConfig::daily_loss_limit)
        .def_readwrite("symbol_position_limits", &speedquant::risk::RiskConfig::symbol_position_limits)
        .def_readwrite("enable_stop_loss", &speedquant::risk::RiskConfig::enable_stop_loss)
        .def_readwrite("enable_take_profit", &speedquant::risk::RiskConfig::enable_take_profit);
    
    py::class_<speedquant::risk::RiskController>(risk, "RiskController")
        .def(py::init<const speedquant::risk::RiskConfig&>())
        .def("check_order", &speedquant::risk::RiskController::check_order)
        .def("update_position", &speedquant::risk::RiskController::update_position)
        .def("get_position", &speedquant::risk::RiskController::get_position)
        .def("get_all_positions", &speedquant::risk::RiskController::get_all_positions)
        .def("add_risk_rule", &speedquant::risk::RiskController::add_risk_rule)
        .def("remove_risk_rule", &speedquant::risk::RiskController::remove_risk_rule)
        .def("enable_risk_rule", &speedquant::risk::RiskController::enable_risk_rule)
        .def("get_all_risk_rules", &speedquant::risk::RiskController::get_all_risk_rules)
        .def("get_total_exposure", &speedquant::risk::RiskController::get_total_exposure)
        .def("get_daily_pnl", &speedquant::risk::RiskController::get_daily_pnl)
        .def("get_max_drawdown", &speedquant::risk::RiskController::get_max_drawdown);
    
    // Utils module
    py::module utils = m.def_submodule("utils", "Utility module");
    
    py::enum_<speedquant::utils::LogLevel>(utils, "LogLevel")
        .value("TRACE", speedquant::utils::LogLevel::TRACE)
        .value("DEBUG", speedquant::utils::LogLevel::DEBUG)
        .value("INFO", speedquant::utils::LogLevel::INFO)
        .value("WARNING", speedquant::utils::LogLevel::WARNING)
        .value("ERROR", speedquant::utils::LogLevel::ERROR)
        .value("FATAL", speedquant::utils::LogLevel::FATAL)
        .export_values();
    
    py::class_<speedquant::utils::LogConfig>(utils, "LogConfig")
        .def(py::init<>())
        .def_readwrite("console_level", &speedquant::utils::LogConfig::console_level)
        .def_readwrite("file_level", &speedquant::utils::LogConfig::file_level)
        .def_readwrite("log_file_path", &speedquant::utils::LogConfig::log_file_path)
        .def_readwrite("enable_console", &speedquant::utils::LogConfig::enable_console)
        .def_readwrite("enable_file", &speedquant::utils::LogConfig::enable_file)
        .def_readwrite("max_file_size", &speedquant::utils::LogConfig::max_file_size)
        .def_readwrite("max_files", &speedquant::utils::LogConfig::max_files);
    
    py::class_<speedquant::utils::Logger>(utils, "Logger", py::dynamic_attr())
        .def_static("instance", &speedquant::utils::Logger::instance, py::return_value_policy::reference)
        .def("configure", &speedquant::utils::Logger::configure)
        .def("log", &speedquant::utils::Logger::log)
        .def("trace", &speedquant::utils::Logger::trace)
        .def("debug", &speedquant::utils::Logger::debug)
        .def("info", &speedquant::utils::Logger::info)
        .def("warning", &speedquant::utils::Logger::warning)
        .def("error", &speedquant::utils::Logger::error)
        .def("fatal", &speedquant::utils::Logger::fatal)
        .def("add_log_handler", &speedquant::utils::Logger::add_log_handler);
    
    // ConfigManager
    py::class_<speedquant::utils::ConfigManager>(utils, "ConfigManager", py::dynamic_attr())
        .def_static("instance", &speedquant::utils::ConfigManager::instance, py::return_value_policy::reference)
        .def("load_from_file", &speedquant::utils::ConfigManager::load_from_file)
        .def("save_to_file", &speedquant::utils::ConfigManager::save_to_file)
        .def("has", &speedquant::utils::ConfigManager::has)
        .def("remove", &speedquant::utils::ConfigManager::remove)
        .def("clear", &speedquant::utils::ConfigManager::clear)
        .def("get_keys", &speedquant::utils::ConfigManager::get_keys)
        .def("add_change_listener", &speedquant::utils::ConfigManager::add_change_listener)
        .def("get_string", [](speedquant::utils::ConfigManager& self, const std::string& key, const std::string& default_value = "") {
            return self.get<std::string>(key, default_value);
        })
        .def("get_int", [](speedquant::utils::ConfigManager& self, const std::string& key, int default_value = 0) {
            return self.get<int>(key, default_value);
        })
        .def("get_double", [](speedquant::utils::ConfigManager& self, const std::string& key, double default_value = 0.0) {
            return self.get<double>(key, default_value);
        })
        .def("get_bool", [](speedquant::utils::ConfigManager& self, const std::string& key, bool default_value = false) {
            return self.get<bool>(key, default_value);
        })
        .def("set_string", [](speedquant::utils::ConfigManager& self, const std::string& key, const std::string& value) {
            self.set<std::string>(key, value);
        })
        .def("set_int", [](speedquant::utils::ConfigManager& self, const std::string& key, int value) {
            self.set<int>(key, value);
        })
        .def("set_double", [](speedquant::utils::ConfigManager& self, const std::string& key, double value) {
            self.set<double>(key, value);
        })
        .def("set_bool", [](speedquant::utils::ConfigManager& self, const std::string& key, bool value) {
            self.set<bool>(key, value);
        });
}
