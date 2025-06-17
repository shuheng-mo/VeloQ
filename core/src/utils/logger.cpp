#include "utils/logger.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <filesystem>

namespace speedquant {

// Initialize static members
std::unique_ptr<Logger> Logger::instance_ = nullptr;
std::once_flag Logger::init_flag_;

Logger& Logger::getInstance() {
    std::call_once(init_flag_, []() {
        instance_.reset(new Logger());
    });
    return *instance_;
}

Logger::Logger() : level_(LogLevel::INFO), console_enabled_(true), file_enabled_(false) {
    // Default configuration
    setPattern("[%Y-%m-%d %H:%M:%S.%ms] [%l] [%t] %v");
}

Logger::~Logger() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

void Logger::configure(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    level_ = config.level;
    console_enabled_ = config.console_enabled;
    file_enabled_ = config.file_enabled;
    pattern_ = config.pattern;
    
    if (file_enabled_) {
        // Close existing file if open
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
        
        // Create directory if it doesn't exist
        std::filesystem::path log_path(config.file_path);
        std::filesystem::create_directories(log_path.parent_path());
        
        // Open log file
        file_stream_.open(config.file_path, std::ios::out | std::ios::app);
        if (!file_stream_.is_open()) {
            std::cerr << "Failed to open log file: " << config.file_path << std::endl;
            file_enabled_ = false;
        }
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    level_ = level;
}

void Logger::setPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(mutex_);
    pattern_ = pattern;
}

void Logger::enableConsole(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    console_enabled_ = enabled;
}

void Logger::enableFile(bool enabled, const std::string& file_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    file_enabled_ = enabled;
    
    if (file_enabled_) {
        // Close existing file if open
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
        
        if (!file_path.empty()) {
            // Create directory if it doesn't exist
            std::filesystem::path log_path(file_path);
            std::filesystem::create_directories(log_path.parent_path());
            
            // Open log file
            file_stream_.open(file_path, std::ios::out | std::ios::app);
            if (!file_stream_.is_open()) {
                std::cerr << "Failed to open log file: " << file_path << std::endl;
                file_enabled_ = false;
            }
        }
    } else {
        // Close file if open
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
    }
}

void Logger::trace(const std::string& message) {
    log(LogLevel::TRACE, message);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Skip if log level is too low
    if (level < level_) {
        return;
    }
    
    // Format the log message
    std::string formatted_message = formatMessage(level, message);
    
    // Output to console if enabled
    if (console_enabled_) {
        // Set console color based on log level
        switch (level) {
            case LogLevel::TRACE:
                std::cout << "\033[90m"; // Dark gray
                break;
            case LogLevel::DEBUG:
                std::cout << "\033[37m"; // White
                break;
            case LogLevel::INFO:
                std::cout << "\033[32m"; // Green
                break;
            case LogLevel::WARN:
                std::cout << "\033[33m"; // Yellow
                break;
            case LogLevel::ERROR:
                std::cout << "\033[31m"; // Red
                break;
            case LogLevel::CRITICAL:
                std::cout << "\033[1;31m"; // Bold red
                break;
        }
        
        std::cout << formatted_message << "\033[0m" << std::endl;
    }
    
    // Output to file if enabled
    if (file_enabled_ && file_stream_.is_open()) {
        file_stream_ << formatted_message << std::endl;
        file_stream_.flush();
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& message) {
    std::string result = pattern_;
    
    // Replace pattern placeholders
    
    // %Y-%m-%d %H:%M:%S.%ms - Timestamp
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() % 1000;
    
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    timestamp << '.' << std::setfill('0') << std::setw(3) << now_ms;
    
    size_t pos = result.find("%Y-%m-%d %H:%M:%S.%ms");
    if (pos != std::string::npos) {
        result.replace(pos, 21, timestamp.str());
    }
    
    // %l - Log level
    pos = result.find("%l");
    if (pos != std::string::npos) {
        result.replace(pos, 2, logLevelToString(level));
    }
    
    // %t - Thread ID
    pos = result.find("%t");
    if (pos != std::string::npos) {
        std::stringstream thread_id;
        thread_id << std::this_thread::get_id();
        result.replace(pos, 2, thread_id.str());
    }
    
    // %v - Message
    pos = result.find("%v");
    if (pos != std::string::npos) {
        result.replace(pos, 2, message);
    }
    
    return result;
}

std::string Logger::logLevelToString(LogLevel level) {
    static const std::map<LogLevel, std::string> level_strings = {
        {LogLevel::TRACE, "TRACE"},
        {LogLevel::DEBUG, "DEBUG"},
        {LogLevel::INFO, "INFO"},
        {LogLevel::WARN, "WARN"},
        {LogLevel::ERROR, "ERROR"},
        {LogLevel::CRITICAL, "CRITICAL"}
    };
    
    auto it = level_strings.find(level);
    if (it != level_strings.end()) {
        return it->second;
    }
    
    return "UNKNOWN";
}

} // namespace speedquant
