#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "obd_service_impl.h"

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    OBD::OBDServiceImpl service;
    
    grpc::EnableDefaultHealthCheckService(true);
    
    grpc::ServerBuilder builder;
    // Listen on the given address without authentication
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register the service
    builder.RegisterService(&service);
    
    // Build and start the server
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "OBD Server listening on " << server_address << std::endl;
    
    // Keep the server running
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
