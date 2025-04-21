#pragma once

#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>

namespace zonal_controller {

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    bool loadConfig(const std::string& configPath);
    
    // Getters for configuration values
    const std::string& getServerAddress() const { return serverAddress; }
    int getServerPort() const { return serverPort; }
    const std::string& getLogFile() const { return logFile; }
    const std::string& getLogLevel() const { return logLevel; }

private:
    Config() = default;
    ~Config() = default;

    std::string serverAddress = "0.0.0.0";
    int serverPort = 50051;
    std::string logFile = "zonal_controller.log";
    std::string logLevel = "INFO";
};

} // namespace zonal_controller 