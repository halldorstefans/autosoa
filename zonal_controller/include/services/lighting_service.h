/**
 * @file lighting_service.h
 * @brief Implementation of the vehicle lighting control gRPC service
 * @author Auto SOA Team
 * @version 1.0.0
 * @date 2024-04-16
 */

#ifndef LIGHTING_SERVICE_H
#define LIGHTING_SERVICE_H

#include <grpcpp/grpcpp.h>
#include "../hardware/body_lights.h"
#include "lighting_service.grpc.pb.h"

namespace Body
{
    /**
     * @class LightingService
     * @brief Implementation of the vehicle lighting control gRPC service
     *
     * This class provides the implementation of the lighting service, which handles:
     * - Headlight state control
     * - Headlight state querying
     * - Error handling and status reporting
     */
    class LightingService final : public lighting::LightingService::Service
    {
    public:
        /**
         * @brief Construct a new Lighting Service object
         *
         * Initializes the body lights controller with default values
         */
        LightingService();

        /**
         * @brief Get the current headlight state
         *
         * @param context Server context for the RPC
         * @param request The headlight state request (unused in current implementation)
         * @param response The response containing current headlight state
         * @return grpc::Status OK on success, INTERNAL on error
         */
        grpc::Status GetHeadlightState(
            grpc::ServerContext *context,
            const lighting::GetHeadlightStateRequest *request,
            lighting::GetHeadlightStateResponse *response) override;

        /**
         * @brief Set the headlight state
         *
         * @param context Server context for the RPC
         * @param request The request containing desired headlight state
         * @param response The response containing operation success status
         * @return grpc::Status OK on success, INTERNAL on error
         */
        grpc::Status SetHeadlight(
            grpc::ServerContext *context,
            const lighting::SetHeadlightRequest *request,
            lighting::SetHeadlightResponse *response) override;

    private:
        Body::Lights body_lights_; ///< Body lights controller instance
    };

} // namespace Body

#endif // LIGHTING_SERVICE_H
