# Learning Service-Oriented Architecture (SOA) in Automotive

A project to learn and understand modern Service-Oriented Architecture (SOA) implementation for vehicle systems, featuring multiple interconnected services for vehicle data management and control.

## System Overview

This project implements a modern automotive software architecture with the following components:

1. **Zonal Controller** (`zonal_controller/`)
   - C++17-based gRPC service
   - Handles OBD (On-Board Diagnostics) functionality
   - Controls vehicle lighting systems
   - Implements real-time fuel level monitoring
   - Manages headlight state control

2. **Signal Service** (`signalservice/`)
   - Go-based gRPC gateway service
   - Provides unified access to vehicle signals
   - Supports both OBD and lighting system signals
   - Configurable gateway with environment variables
   - Implements middleware for request handling

3. **Vehicle Dashboard** (`dashboard/`)
   - Flask-based web interface
   - Real-time vehicle data monitoring
   - Vehicle feature control
   - Server-Sent Events (SSE) for live updates
   - RESTful API endpoints

## Architecture

The system follows a service-oriented architecture with clear separation of concerns:

```
├── zonal_controller/     # Low-level vehicle control
├── signalservice/        # Signal gateway and management
└── dashboard/            # Web interface and monitoring
```

## Dependencies

### For C++ Services (Zonal Controller)
```bash
sudo apt update
sudo apt install -y build-essential cmake
sudo apt install -y libgrpc++-dev libgrpc-dev
sudo apt install -y protobuf-compiler protobuf-compiler-grpc libprotobuf-dev
```

### For Go Services (Signal Service)
```bash
sudo apt update
sudo apt install golang-go
sudo apt install -y protobuf-compiler

# Required Go packages
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
export PATH="$PATH:$(go env GOPATH)/bin"
go get google.golang.org/grpc
```

### For Python Services (Dashboard)
```bash
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

## Building and Running

### 1. Build the Zonal Controller
```bash
cd zonal_controller
rm -rf build
mkdir build
cd build
cmake ..
make
```

### 2. Build the Signal Service
```bash
cd signalservice
go mod download
go build -o signalservice cmd/signalservice/main.go
```

### 3. Build the Dashboard
```bash
cd dashboard
pip install -r requirements.txt
```

## Running the System

1. Start the Zonal Controller:
```bash
cd zonal_controller/build
./zonal_controller
```

2. Start the Signal Service:
```bash
cd signalservice
./signalservice
```

3. Start the Dashboard:
```bash
cd dashboard
python3 app.py
```

## Configuration

Each component can be configured through environment variables:

- **Signal Service**:
  - `OB_SERVER_ADDR`: OBD server address (default: "localhost:50051")
  - `DEFAULT_VEHICLE`: Default vehicle identifier
  - `GATEWAY_PORT`: Gateway service port (default: 8080)

- **Dashboard**:
  - `DEBUG`: Enable/disable debug mode
  - `PORT`: Server port (default: 5000)
  - `VEHICLE_GATEWAY_URL`: V2C gateway URL

## Development Status

The system is currently in active development with the following features implemented:

- Real-time vehicle data monitoring
- Vehicle lighting control
- OBD functionality
- Web-based dashboard interface
- gRPC-based service communication
- Protocol Buffers for message serialization
- Configuration management
- Logging system
- Security features

## Potential Future Improvements

- Comprehensive API documentation
- Additional vehicle data points
- Enhanced error handling and logging
- User authentication and authorization
- Historical data visualization
- CI/CD pipeline configuration
- Comprehensive testing suite
- Metrics and monitoring
