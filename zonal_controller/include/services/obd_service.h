/**
 * @file obd_service.h
 * @brief Implementation of the OBD (On-Board Diagnostics) gRPC service
 * @author Auto SOA Team
 * @version 1.0.0
 * @date 2024-04-16
 */

#ifndef OBD_SERVICE_H
#define OBD_SERVICE_H

#include <grpcpp/grpcpp.h>
#include "../hardware/fuel_level_sensor.h"
#include "obd_service.grpc.pb.h"
#include <chrono>

namespace OBD
{
    /**
     * @class OBDService
     * @brief Implementation of the OBD gRPC service interface
     *
     * This class provides the implementation of the OBD service, which handles:
     * - Real-time fuel level monitoring
     * - Fuel level streaming
     * - Error handling and status reporting
     */
    class OBDService final : public obd::OBDService::Service
    {
    public:
        /**
         * @brief Construct a new OBDService object
         *
         * Initializes the fuel level sensor with default values and sets up
         * necessary resources for the service.
         */
        OBDService();

        /**
         * @brief Get the current fuel level
         *
         * @param context Server context for the RPC
         * @param request The fuel level request (unused in current implementation)
         * @param response The response containing current fuel level and status
         * @return grpc::Status OK on success, INTERNAL on error
         */
        grpc::Status GetFuelLevel(grpc::ServerContext *context,
                                  const obd::FuelLevelRequest *request,
                                  obd::FuelLevelResponse *response) override;

        /**
         * @brief Stream fuel level updates
         *
         * @param context Server context for the RPC
         * @param request The stream request containing update interval
         * @param writer Writer for streaming fuel level responses
         * @return grpc::Status OK on success, INTERNAL on error
         */
        grpc::Status StreamFuelLevel(grpc::ServerContext *context,
                                     const obd::FuelLevelStreamRequest *request,
                                     grpc::ServerWriter<obd::FuelLevelResponse> *writer) override;

    private:
        OBD::FuelLevelSensor fuel_sensor_; ///< Fuel level sensor instance

        /**
         * @brief Create a fuel level response message
         *
         * @param level Current fuel level percentage
         * @return obd::FuelLevelResponse Response message with timestamp and status
         */
        obd::FuelLevelResponse CreateFuelLevelResponse(float level);
    };

} // namespace OBD

#endif // OBD_SERVICE_H
