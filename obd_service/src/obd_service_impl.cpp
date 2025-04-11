#include "obd_service_impl.h"
#include <thread>
#include <ctime>

namespace OBD {

OBDServiceImpl::OBDServiceImpl() : fuel_sensor_(75.0f, 0.01f) {
    // Initialize the fuel sensor with default values
}

grpc::Status OBDServiceImpl::GetFuelLevel(grpc::ServerContext* context,
                                         const obd::FuelLevelRequest* request,
                                         obd::FuelLevelResponse* response) {
    // Lock to ensure thread safety when accessing the sensor
    std::lock_guard<std::mutex> lock(sensor_mutex_);
    
    try {
        // Read the fuel level from the sensor
        float level = fuel_sensor_.read_fuel_level();
        *response = CreateFuelLevelResponse(level);
        return grpc::Status::OK;
    } catch (const std::exception& e) {
        // Handle any exceptions
        response->set_status(1);
        response->set_error_message(e.what());
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status OBDServiceImpl::StreamFuelLevel(grpc::ServerContext* context,
                                           const obd::FuelLevelStreamRequest* request,
                                           grpc::ServerWriter<obd::FuelLevelResponse>* writer) {
    // Get the requested interval (default to 1 second if 0)
    uint32_t interval_seconds = request->interval_seconds();
    if (interval_seconds == 0) {
        interval_seconds = 1;
    }
    
    // Stream fuel level updates until client disconnects
    while (!context->IsCancelled()) {
        obd::FuelLevelResponse response;
        
        {
            // Lock only during sensor access
            std::lock_guard<std::mutex> lock(sensor_mutex_);
            float level = fuel_sensor_.read_fuel_level();
            response = CreateFuelLevelResponse(level);
        }
        
        // Write the response to the stream
        if (!writer->Write(response)) {
            break;
        }
        
        // Wait for the specified interval
        std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
    }
    
    return grpc::Status::OK;
}

obd::FuelLevelResponse OBDServiceImpl::CreateFuelLevelResponse(float level) {
    obd::FuelLevelResponse response;
    
    // Set the fuel level
    response.set_level_percent(level);
    
    // Set the current timestamp in milliseconds
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch().count();
    response.set_timestamp_ms(value);
    
    // Set status to OK
    response.set_status(0);
    
    return response;
}

} // namespace OBD
