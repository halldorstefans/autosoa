/**
 * @file server_main.cpp
 * @brief Main entry point for the OBD and Lighting gRPC server
 * @author Auto SOA Team
 * @version 1.0.0
 * @date 2024-04-16
 */

#include <iostream>
#include <memory>
#include <string>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "services/obd_service.h"
#include "services/lighting_service.h"
#include "logger.hpp"
#include "config.hpp"

namespace {
    std::atomic<bool> g_running{true};
    std::unique_ptr<grpc::Server> g_server;

    void signalHandler(int signal) {
        LOG_INFO("Received signal {}. Shutting down gracefully...", signal);
        g_running = false;
        if (g_server) {
            g_server->Shutdown();
        }
    }
}

/**
 * @brief Run the gRPC server
 *
 * This function:
 * 1. Creates service instances
 * 2. Configures the gRPC server
 * 3. Starts the server
 * 4. Keeps the server running until shutdown
 *
 * @note The server runs on port 50051 and listens on all interfaces
 */
void RunServer()
{
    auto& config = zonal_controller::Config::getInstance();
    std::string server_address = config.getServerAddress() + ":" + std::to_string(config.getServerPort());
    
    OBD::OBDService obd_service;
    Body::LightingService light_service;

    LOG_INFO("Initializing gRPC server on {}", server_address);
    
    grpc::EnableDefaultHealthCheckService(true);

    grpc::ServerBuilder builder;
    // Listen on the given address without authentication
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register the service
    builder.RegisterService(&obd_service);
    builder.RegisterService(&light_service);

    // Build and start the server
    g_server = builder.BuildAndStart();
    LOG_INFO("OBD Server listening on {}", server_address);

    // Keep the server running until shutdown
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/**
 * @brief Main entry point
 *
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return int Exit code (0 on success)
 */
int main(int argc, char **argv)
{
    // Initialize logging
    auto& logger = zonal_controller::Logger::getInstance();
    logger.setLogLevel(zonal_controller::LogLevel::DEBUG);
    logger.setLogFile("zonal_controller.log");
    
    // Load configuration
    auto& config = zonal_controller::Config::getInstance();
    if (!config.loadConfig("config.yaml")) {
        LOG_ERROR("Failed to load configuration. Using defaults.");
    }

    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    LOG_INFO("Starting Zonal Controller Server");
    try {
        RunServer();
    } catch (const std::exception& e) {
        LOG_ERROR("Server error: {}", e.what());
        return 1;
    }

    LOG_INFO("Server shutdown complete");
    return 0;
}
