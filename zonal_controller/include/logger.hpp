#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace zonal_controller {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogLevel(LogLevel level) {
        log_level_ = level;
    }

    void setLogFile(const std::string& filename) {
        log_file_.open(filename, std::ios::app);
    }

    template<typename... Args>
    void debug(const char* file, int line, const std::string& format, Args... args) {
        log(LogLevel::DEBUG, file, line, format, args...);
    }

    template<typename... Args>
    void info(const char* file, int line, const std::string& format, Args... args) {
        log(LogLevel::INFO, file, line, format, args...);
    }

    template<typename... Args>
    void warning(const char* file, int line, const std::string& format, Args... args) {
        log(LogLevel::WARNING, file, line, format, args...);
    }

    template<typename... Args>
    void error(const char* file, int line, const std::string& format, Args... args) {
        log(LogLevel::ERROR, file, line, format, args...);
    }

private:
    Logger() : log_level_(LogLevel::INFO) {}
    ~Logger() {
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template<typename... Args>
    void log(LogLevel level, const char* file, int line, const std::string& format, Args... args) {
        if (level < log_level_) return;

        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

        std::string level_str;
        switch (level) {
            case LogLevel::DEBUG: level_str = "DEBUG"; break;
            case LogLevel::INFO: level_str = "INFO"; break;
            case LogLevel::WARNING: level_str = "WARNING"; break;
            case LogLevel::ERROR: level_str = "ERROR"; break;
        }

        // Remove base path
        std::string fileStr(file);
        std::string fileSubStr = fileStr.substr(33);

        std::lock_guard<std::mutex> lock(mutex_);
        std::string message = formatMessage(format, args...);
        std::string log_entry = ss.str() + " [" + level_str + "] " + fileSubStr + ":" + 
                              std::to_string(line) + " - " + message + "\n";

        std::cout << log_entry;
        if (log_file_.is_open()) {
            log_file_ << log_entry;
            log_file_.flush();
        }
    }

    // Base case for no arguments
    std::string formatMessage(const std::string& format) {
        return format;
    }

    // Specialization for const char*
    std::string formatMessage(const std::string& format, const char* value) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) return format;
        return format.substr(0, pos) + value + format.substr(pos + 2);
    }

    // Specialization for std::string
    std::string formatMessage(const std::string& format, const std::string& value) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) return format;
        return format.substr(0, pos) + value + format.substr(pos + 2);
    }

    // Specialization for bool
    std::string formatMessage(const std::string& format, bool value) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) return format;
        return format.substr(0, pos) + (value ? "true" : "false") + format.substr(pos + 2);
    }

    // Generic case for numeric types
    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
    formatMessage(const std::string& format, T value) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) return format;
        return format.substr(0, pos) + std::to_string(value) + format.substr(pos + 2);
    }

    // Recursive case for multiple arguments
    template<typename T, typename... Args>
    std::string formatMessage(const std::string& format, T value, Args... args) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) return format;
        return formatMessage(format.substr(0, pos) + formatMessage("{}", value) + format.substr(pos + 2), args...);
    }

    std::mutex mutex_;
    std::ofstream log_file_;
    LogLevel log_level_;
};

} // namespace zonal_controller

// Convenience macros
#define LOG_DEBUG(...) zonal_controller::Logger::getInstance().debug(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) zonal_controller::Logger::getInstance().info(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) zonal_controller::Logger::getInstance().warning(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) zonal_controller::Logger::getInstance().error(__FILE__, __LINE__, __VA_ARGS__) 