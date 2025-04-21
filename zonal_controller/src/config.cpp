#include "config.hpp"
#include "logger.hpp"
#include <fstream>
#include <filesystem>

namespace zonal_controller {

bool Config::loadConfig(const std::string& configPath) {
    // Try to find the config file in multiple locations
    std::vector<std::string> possiblePaths = {
        configPath,  // Try the provided path first
        "config.yaml",  // Try current directory
        "/etc/zonal_controller/config.yaml"  // Try system directory
    };

    std::string foundPath;
    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path)) {
            foundPath = path;
            break;
        }
    }

    if (foundPath.empty()) {
        LOG_ERROR("Could not find config.yaml in any of the following locations:");
        for (const auto& path : possiblePaths) {
            LOG_ERROR("  - {}", path);
        }
        return false;
    }

    try {
        YAML::Node config = YAML::LoadFile(foundPath);
        
        if (config["server"]) {
            if (config["server"]["address"]) {
                serverAddress = config["server"]["address"].as<std::string>();
            }
            if (config["server"]["port"]) {
                serverPort = config["server"]["port"].as<int>();
            }
        }

        if (config["logging"]) {
            if (config["logging"]["file"]) {
                logFile = config["logging"]["file"].as<std::string>();
            }
            if (config["logging"]["level"]) {
                logLevel = config["logging"]["level"].as<std::string>();
            }
        }

        LOG_INFO("Configuration loaded successfully from {}", foundPath);
        return true;
    } catch (const YAML::Exception& e) {
        LOG_ERROR("Failed to load configuration: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error loading configuration: {}", e.what());
        return false;
    }
}

} // namespace zonal_controller 