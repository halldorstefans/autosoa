#include "../include/services/obd_service.h"
#include <thread>
#include <ctime>
#include "../include/logger.hpp"

namespace OBD
{

    OBDService::OBDService() : fuel_sensor_(75.0f, 0.01f)
    {
        LOG_INFO("Initializing OBD service with default fuel level: {}%", 75.0f);
    }

    grpc::Status OBDService::GetFuelLevel(grpc::ServerContext *context,
                                          const obd::FuelLevelRequest *request,
                                          obd::FuelLevelResponse *response)
    {
        try
        {
            LOG_DEBUG("Received GetFuelLevel request");
            // Read the fuel level from the sensor
            float level = fuel_sensor_.read_fuel_level();
            *response = CreateFuelLevelResponse(level);
            LOG_INFO("Fuel level read: {}%", level);
            return grpc::Status::OK;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Error reading fuel level: {}", e.what());
            // Handle any exceptions
            response->set_status(1);
            response->set_error_message(e.what());
            return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    grpc::Status OBDService::StreamFuelLevel(grpc::ServerContext *context,
                                             const obd::FuelLevelStreamRequest *request,
                                             grpc::ServerWriter<obd::FuelLevelResponse> *writer)
    {
        LOG_INFO("Starting fuel level stream with interval: {} seconds", request->interval_seconds());
        // Get the requested interval (default to 1 second if 0)
        uint32_t interval_seconds = request->interval_seconds();
        if (interval_seconds == 0)
        {
            interval_seconds = 1;
            LOG_WARNING("Stream interval was 0, defaulting to 1 second");
        }

        // Stream fuel level updates until client disconnects
        while (!context->IsCancelled())
        {
            obd::FuelLevelResponse response;

            {
                float level = fuel_sensor_.read_fuel_level();
                response = CreateFuelLevelResponse(level);
                LOG_DEBUG("Streaming fuel level: {}%", level);
            }

            // Write the response to the stream
            if (!writer->Write(response))
            {
                LOG_INFO("Client disconnected from fuel level stream");
                break;
            }

            // Wait for the specified interval
            std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
        }

        return grpc::Status::OK;
    }

    obd::FuelLevelResponse OBDService::CreateFuelLevelResponse(float level)
    {
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
