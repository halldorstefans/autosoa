# Zonal Controller

A gRPC-based service implementing vehicle OBD (On-Board Diagnostics) and lighting control functionality.

## Overview

This project provides a zonal controller implementation that manages:
- OBD functionality (fuel level monitoring)
- Vehicle lighting control (headlights)

The system is built using:
- C++17
- gRPC for service communication
- Protocol Buffers for message serialization
- YAML-CPP for configuration management

## Features

### OBD Service
- Real-time fuel level monitoring
- Fuel level streaming with configurable update intervals
- Error handling and status reporting

### Lighting Service
- Headlight state control
- Headlight state querying
- Error handling and status reporting

## Building

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler
- gRPC and Protocol Buffers
- pkg-config
- yaml-cpp

### Build Steps
```bash
mkdir build
cd build
cmake ..
make
```

The build system includes:
- Compiler optimizations (-O2)
- Debug symbols (-g)
- Security features:
  - Stack protection
  - Memory safety
  - Control flow protection
- Strict compiler warnings

## Running

The server can be started with:
```bash
./zonal_controller
```

The server will listen on port 50051 by default and uses config.yaml for configuration.

## API Documentation

### OBD Service
- `GetFuelLevel`: Returns current fuel level
- `StreamFuelLevel`: Streams fuel level updates at specified intervals

### Lighting Service
- `GetHeadlightState`: Returns current headlight state
- `SetHeadlight`: Controls headlight state (on/off)

## Project Structure

```
zonal_controller/
├── include/            # Header files
│   ├── hardware/       # Hardware interface headers
│   ├── services/       # Service implementation headers
│   ├── config.hpp      # Configuration management
│   ├── logger.hpp      # Logging utilities
│   └── version.h.in    # Version information template
├── src/                # Source files
│   ├── hardware/       # Hardware implementation
│   ├── services/       # Service implementations
│   ├── config.cpp      # Configuration implementation
│   └── server_main.cpp # Main server entry point
├── build/              # Build directory
├── CMakeLists.txt      # Build configuration
├── config.yaml         # Configuration file
└── README.md           # This file
```

## Dependencies

- gRPC
- Protocol Buffers
- yaml-cpp
- C++ Standard Library
- pthread

## Version History

- 1.0.0 (2024-04-16): Initial release
  - Basic OBD and lighting services
  - Configuration management
  - Logging system
  - Security features
  - Build system with optimizations 