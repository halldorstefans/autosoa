#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "obd_service.grpc.pb.h"

class OBDClient {
public:
    OBDClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(obd::OBDService::NewStub(channel)) {}
    
    // Get a single fuel level reading
    void GetFuelLevel(const std::string& vehicle_id) {
        // Create request
        obd::FuelLevelRequest request;
        request.set_vehicle_id(vehicle_id);
        
        // Response object
        obd::FuelLevelResponse response;
        
        // Context for the client
        grpc::ClientContext context;
        
        // Make the call
        grpc::Status status = stub_->GetFuelLevel(&context, request, &response);
        
        if (status.ok()) {
            std::cout << "Fuel level: " << response.level_percent() << "%" << std::endl;
            std::cout << "Timestamp: " << response.timestamp_ms() << std::endl;
            
            // Create JSON format output
            std::cout << "JSON: {\"level_percent\": " << response.level_percent() 
                      << ", \"timestamp_ms\": " << response.timestamp_ms() 
                      << ", \"status\": " << response.status() << "}" << std::endl;
        } else {
            std::cout << "RPC failed: " << status.error_message() << std::endl;
        }
    }
    
    // Stream fuel level updates
    void StreamFuelLevel(const std::string& vehicle_id, int interval_seconds) {
        // Create request
        obd::FuelLevelStreamRequest request;
        request.set_vehicle_id(vehicle_id);
        request.set_interval_seconds(interval_seconds);
        
        // Context for the client
        grpc::ClientContext context;
        
        // Stream reader for the responses
        std::unique_ptr<grpc::ClientReader<obd::FuelLevelResponse>> reader(
            stub_->StreamFuelLevel(&context, request));
        
        // Read responses
        obd::FuelLevelResponse response;
        int count = 0;
        while (reader->Read(&response) && count < 10) { // Limit to 10 readings for demo
            std::cout << "Fuel level update: " << response.level_percent() << "%" << std::endl;
            std::cout << "JSON: {\"level_percent\": " << response.level_percent() 
                      << ", \"timestamp_ms\": " << response.timestamp_ms() 
                      << ", \"status\": " << response.status() << "}" << std::endl;
            count++;
        }
        
        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "StreamFuelLevel RPC failed: " << status.error_message() << std::endl;
        }
    }
    
private:
    std::unique_ptr<obd::OBDService::Stub> stub_;
};

int main(int argc, char** argv) {
    // Create a client
    OBDClient client(grpc::CreateChannel(
        "localhost:50051", grpc::InsecureChannelCredentials()));
    
    std::string vehicle_id = "VIN123456789";
    
    // Get a single fuel level reading
    std::cout << "Getting fuel level..." << std::endl;
    client.GetFuelLevel(vehicle_id);
    
    // Stream fuel level updates
    std::cout << "\nStreaming fuel level updates (every 2 seconds)..." << std::endl;
    client.StreamFuelLevel(vehicle_id, 2);
    
    return 0;
}
