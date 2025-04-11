#ifndef OBD_SERVICE_IMPL_H
#define OBD_SERVICE_IMPL_H

#include <grpcpp/grpcpp.h>
#include "fuel_level_sensor.h"
#include "obd_service.grpc.pb.h"
#include <chrono>
#include <mutex>

namespace OBD {

class OBDServiceImpl final : public obd::OBDService::Service {
public:
    OBDServiceImpl();
    
    // Implement the GetFuelLevel method
    grpc::Status GetFuelLevel(grpc::ServerContext* context,
                             const obd::FuelLevelRequest* request,
                             obd::FuelLevelResponse* response) override;
                             
    // Implement the StreamFuelLevel method
    grpc::Status StreamFuelLevel(grpc::ServerContext* context,
                                const obd::FuelLevelStreamRequest* request,
                                grpc::ServerWriter<obd::FuelLevelResponse>* writer) override;
                                
private:
    ECU::FuelLevelSensor fuel_sensor_;
    std::mutex sensor_mutex_;  // To ensure thread safety
    
    // Helper method to create a fuel level response
    obd::FuelLevelResponse CreateFuelLevelResponse(float level);
};

} // namespace OBD

#endif // OBD_SERVICE_IMPL_H
