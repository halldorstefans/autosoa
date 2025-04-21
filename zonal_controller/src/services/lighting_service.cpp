#include "../include/services/lighting_service.h"
#include "../include/logger.hpp"

namespace Body
{

  LightingService::LightingService() : body_lights_()
  {
    LOG_INFO("Initializing Lighting service");
  }

  grpc::Status LightingService::GetHeadlightState(
      grpc::ServerContext *context,
      const lighting::GetHeadlightStateRequest *request,
      lighting::GetHeadlightStateResponse *response)
  {
    try
    {
      LOG_DEBUG("Received GetHeadlightState request");
      // Call the embedded function to get state
      bool state = body_lights_.get_headlight_state();

      // Convert to boolean and set response
      response->set_is_on(state == 1);
      LOG_INFO("Headlight state: {}", state ? "ON" : "OFF");

      return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
      LOG_ERROR("Error getting headlight state: {}", e.what());
      // Handle any exceptions
      return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
  }

  grpc::Status LightingService::SetHeadlight(
      grpc::ServerContext *context,
      const lighting::SetHeadlightRequest *request,
      lighting::SetHeadlightResponse *response)
  {
    try
    {
      LOG_DEBUG("Received SetHeadlight request: {}", request->turn_on() ? "ON" : "OFF");
      // Convert boolean to int (1 = ON, 0 = OFF)
      bool state_to_set = request->turn_on() ? 1 : 0;

      // Call the embedded function to set state
      bool result = body_lights_.set_headlight(state_to_set);

      // Set response based on result
      response->set_success(result == 1);
      
      if (result) {
        LOG_INFO("Successfully set headlight state to: {}", state_to_set ? "ON" : "OFF");
      } else {
        LOG_WARNING("Failed to set headlight state to: {}", state_to_set ? "ON" : "OFF");
      }

      return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
      LOG_ERROR("Error setting headlight state: {}", e.what());
      // Handle any exceptions
      return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
  }
}
