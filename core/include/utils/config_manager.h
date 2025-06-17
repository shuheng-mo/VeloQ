#pragma once

#include <string>
#include <map>
#include <any>
#include <mutex>
#include <memory>
#include <vector>
#include <functional>

namespace speedquant {
namespace utils {

class ConfigManager {
public:
    static ConfigManager& instance();
    
    // Load configuration from file
    bool load_from_file(const std::string& file_path);
    
    // Save configuration to file
    bool save_to_file(const std::string& file_path);
    
    // Get configuration values
    template<typename T>
    T get(const std::string& key, const T& default_value = T()) const;
    
    // Set configuration values
    template<typename T>
    void set(const std::string& key, const T& value);
    
    // Check if a key exists
    bool has(const std::string& key) const;
    
    // Remove a key
    void remove(const std::string& key);
    
    // Clear all configurations
    void clear();
    
    // Get all keys
    std::vector<std::string> get_keys() const;
    
    // Configuration change notification
    using ConfigChangeCallback = std::function<void(const std::string&, const std::any&)>;
    void add_change_listener(ConfigChangeCallback callback);
    
private:
    ConfigManager();
    ~ConfigManager();
    
    // Non-copyable
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    mutable std::mutex mutex_;
    std::map<std::string, std::any> config_values_;
    std::vector<ConfigChangeCallback> change_listeners_;
    
    // Helper methods
    void notify_change(const std::string& key, const std::any& value);
};

// Template specializations for common types
template<>
inline std::string ConfigManager::get<std::string>(const std::string& key, const std::string& default_value) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        try {
            return std::any_cast<std::string>(it->second);
        } catch (const std::bad_any_cast&) {
            return default_value;
        }
    }
    return default_value;
}

template<>
inline int ConfigManager::get<int>(const std::string& key, const int& default_value) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        try {
            return std::any_cast<int>(it->second);
        } catch (const std::bad_any_cast&) {
            return default_value;
        }
    }
    return default_value;
}

template<>
inline double ConfigManager::get<double>(const std::string& key, const double& default_value) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        try {
            return std::any_cast<double>(it->second);
        } catch (const std::bad_any_cast&) {
            return default_value;
        }
    }
    return default_value;
}

template<>
inline bool ConfigManager::get<bool>(const std::string& key, const bool& default_value) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        try {
            return std::any_cast<bool>(it->second);
        } catch (const std::bad_any_cast&) {
            return default_value;
        }
    }
    return default_value;
}

} // namespace utils
} // namespace speedquant
