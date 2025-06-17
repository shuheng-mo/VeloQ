#include "utils/config_manager.h"
#include "utils/logger.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace speedquant {

// Initialize static members
std::unique_ptr<ConfigManager> ConfigManager::instance_ = nullptr;
std::once_flag ConfigManager::init_flag_;

ConfigManager& ConfigManager::getInstance() {
    std::call_once(init_flag_, []() {
        instance_.reset(new ConfigManager());
    });
    return *instance_;
}

ConfigManager::ConfigManager() {
    Logger::getInstance().info("ConfigManager initialized");
}

ConfigManager::~ConfigManager() {
    Logger::getInstance().info("ConfigManager destroyed");
}

bool ConfigManager::loadFromFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Check if file exists
        if (!std::filesystem::exists(file_path)) {
            Logger::getInstance().error("Config file not found: {}", file_path);
            return false;
        }
        
        // Open the file
        std::ifstream file(file_path);
        if (!file.is_open()) {
            Logger::getInstance().error("Failed to open config file: {}", file_path);
            return false;
        }
        
        // Parse the file as JSON
        nlohmann::json config_json;
        file >> config_json;
        
        // Clear existing config
        config_.clear();
        
        // Load the config
        parseJsonConfig(config_json);
        
        Logger::getInstance().info("Loaded configuration from file: {}", file_path);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error loading config from file: {}", e.what());
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Create directory if it doesn't exist
        std::filesystem::path config_path(file_path);
        std::filesystem::create_directories(config_path.parent_path());
        
        // Open the file
        std::ofstream file(file_path);
        if (!file.is_open()) {
            Logger::getInstance().error("Failed to open config file for writing: {}", file_path);
            return false;
        }
        
        // Convert config to JSON
        nlohmann::json config_json = configToJson();
        
        // Write to file with pretty formatting
        file << config_json.dump(4);
        
        Logger::getInstance().info("Saved configuration to file: {}", file_path);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error saving config to file: {}", e.what());
        return false;
    }
}

bool ConfigManager::loadFromJson(const std::string& json_string) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Parse the JSON string
        nlohmann::json config_json = nlohmann::json::parse(json_string);
        
        // Clear existing config
        config_.clear();
        
        // Load the config
        parseJsonConfig(config_json);
        
        Logger::getInstance().info("Loaded configuration from JSON string");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error loading config from JSON: {}", e.what());
        return false;
    }
}

std::string ConfigManager::saveToJson() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Convert config to JSON
        nlohmann::json config_json = configToJson();
        
        // Return as string with pretty formatting
        return config_json.dump(4);
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error saving config to JSON: {}", e.what());
        return "{}";
    }
}

bool ConfigManager::hasKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_.find(key) != config_.end();
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> keys;
    keys.reserve(config_.size());
    
    for (const auto& [key, _] : config_) {
        keys.push_back(key);
    }
    
    return keys;
}

std::vector<std::string> ConfigManager::getKeysWithPrefix(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> keys;
    
    for (const auto& [key, _] : config_) {
        if (key.substr(0, prefix.size()) == prefix) {
            keys.push_back(key);
        }
    }
    
    return keys;
}

void ConfigManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    config_.clear();
    Logger::getInstance().info("Configuration cleared");
}

bool ConfigManager::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = config_.find(key);
    if (it == config_.end()) {
        Logger::getInstance().warn("Cannot remove config: Key '{}' not found", key);
        return false;
    }
    
    config_.erase(it);
    
    // Notify observers
    notifyObservers(key);
    
    Logger::getInstance().info("Removed config key: {}", key);
    return true;
}

void ConfigManager::registerObserver(const std::string& key, ConfigObserver observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    observers_[key].push_back(observer);
    Logger::getInstance().debug("Registered observer for key: {}", key);
}

void ConfigManager::unregisterObserver(const std::string& key, ConfigObserver observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = observers_.find(key);
    if (it == observers_.end()) {
        return;
    }
    
    // Find and remove the observer
    auto& observers = it->second;
    observers.erase(
        std::remove_if(observers.begin(), observers.end(),
            [&observer](const ConfigObserver& o) {
                return o.target_type() == observer.target_type() &&
                       o.target<void(const std::string&, const ConfigValue&)>() == 
                       observer.target<void(const std::string&, const ConfigValue&)>();
            }),
        observers.end());
    
    Logger::getInstance().debug("Unregistered observer for key: {}", key);
}

void ConfigManager::notifyObservers(const std::string& key) {
    // Find observers for this specific key
    auto it = observers_.find(key);
    if (it != observers_.end()) {
        for (const auto& observer : it->second) {
            observer(key, config_[key]);
        }
    }
    
    // Find observers for all keys (wildcard "*")
    it = observers_.find("*");
    if (it != observers_.end()) {
        for (const auto& observer : it->second) {
            observer(key, config_[key]);
        }
    }
}

void ConfigManager::parseJsonConfig(const nlohmann::json& json, const std::string& prefix) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it->is_object()) {
            // Recursively parse nested objects
            parseJsonConfig(*it, key);
        } else {
            // Store the value
            if (it->is_string()) {
                config_[key] = it->get<std::string>();
            } else if (it->is_number_integer()) {
                config_[key] = it->get<int64_t>();
            } else if (it->is_number_float()) {
                config_[key] = it->get<double>();
            } else if (it->is_boolean()) {
                config_[key] = it->get<bool>();
            } else if (it->is_array()) {
                // Convert array to string
                config_[key] = it->dump();
            } else {
                // Convert other types to string
                config_[key] = it->dump();
            }
        }
    }
}

nlohmann::json ConfigManager::configToJson() {
    nlohmann::json result;
    
    for (const auto& [key, value] : config_) {
        // Split the key by dots
        std::vector<std::string> parts;
        std::string part;
        std::istringstream key_stream(key);
        
        while (std::getline(key_stream, part, '.')) {
            parts.push_back(part);
        }
        
        // Build the nested JSON structure
        nlohmann::json* current = &result;
        
        for (size_t i = 0; i < parts.size() - 1; ++i) {
            if (!current->contains(parts[i])) {
                (*current)[parts[i]] = nlohmann::json::object();
            }
            current = &(*current)[parts[i]];
        }
        
        // Set the value
        std::string last_part = parts.back();
        
        std::visit([&current, &last_part](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                // Check if the string is a JSON array or object
                if ((arg.front() == '[' && arg.back() == ']') ||
                    (arg.front() == '{' && arg.back() == '}')) {
                    try {
                        (*current)[last_part] = nlohmann::json::parse(arg);
                    } catch (...) {
                        // If parsing fails, store as string
                        (*current)[last_part] = arg;
                    }
                } else {
                    (*current)[last_part] = arg;
                }
            } else {
                (*current)[last_part] = arg;
            }
        }, value);
    }
    
    return result;
}

} // namespace speedquant
