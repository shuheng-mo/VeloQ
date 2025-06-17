#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <mutex>
#include <vector>
#include <functional>

namespace speedquant {
namespace utils {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

struct LogConfig {
    LogLevel console_level = LogLevel::INFO;
    LogLevel file_level = LogLevel::DEBUG;
    std::string log_file_path = "logs/speedquant.log";
    bool enable_console = true;
    bool enable_file = true;
    size_t max_file_size = 10 * 1024 * 1024;  // 10MB
    int max_files = 5;
};

class Logger {
public:
    static Logger& instance();
    
    void configure(const LogConfig& config);
    
    void log(LogLevel level, const std::string& message);
    
    // Convenience methods
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // Custom log handler
    using LogHandler = std::function<void(LogLevel, const std::string&)>;
    void add_log_handler(LogHandler handler);
    
private:
    Logger();
    ~Logger();
    
    // Non-copyable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    LogConfig config_;
    std::mutex mutex_;
    std::ofstream log_file_;
    std::vector<LogHandler> handlers_;
    
    // Helper methods
    void open_log_file();
    void rotate_log_file_if_needed();
    std::string level_to_string(LogLevel level);
};

// Macro helpers for logging
#define SQ_LOG_TRACE(message) speedquant::utils::Logger::instance().trace(message)
#define SQ_LOG_DEBUG(message) speedquant::utils::Logger::instance().debug(message)
#define SQ_LOG_INFO(message) speedquant::utils::Logger::instance().info(message)
#define SQ_LOG_WARNING(message) speedquant::utils::Logger::instance().warning(message)
#define SQ_LOG_ERROR(message) speedquant::utils::Logger::instance().error(message)
#define SQ_LOG_FATAL(message) speedquant::utils::Logger::instance().fatal(message)

} // namespace utils
} // namespace speedquant
