cmake_minimum_required(VERSION 3.10)
project(zonal_controller VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compiler warnings and optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-O2)  # Optimize for performance
    add_compile_options(-g)   # Include debug symbols
endif()

# Resource limits and monitoring
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Stack protection
    add_compile_options(-fstack-protector-strong)
    
    # Memory safety
    add_compile_options(-D_FORTIFY_SOURCE=2)
    
    # Control flow protection
    add_compile_options(-fcf-protection=full)
endif()

# Allow unused parameters in main() function
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wno-unused-parameter)
endif()

# Find packages using pkg-config (more reliable on Ubuntu/Pop OS)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROTOBUF REQUIRED protobuf)
pkg_check_modules(GRPC REQUIRED grpc++)

# Find required packages
find_package(yaml-cpp REQUIRED)

# Find protoc and grpc_cpp_plugin executables
find_program(PROTOBUF_PROTOC_EXECUTABLE protoc)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

if(NOT PROTOBUF_PROTOC_EXECUTABLE OR NOT GRPC_CPP_PLUGIN_EXECUTABLE)
    message(FATAL_ERROR "Could not find protoc or grpc_cpp_plugin executables")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hardware
    ${CMAKE_CURRENT_SOURCE_DIR}/include/services
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    ${yaml-cpp_INCLUDE_DIRS}
)

# Define proto folder and relative file paths
set(PROTO_DIR "${CMAKE_SOURCE_DIR}/../proto")
set(PROTO_FILES
    "obd_service.proto"
    "lighting_service.proto"
)

# Generate protobuf and gRPC files for each proto file
foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    add_custom_command(
        OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.h"
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS
            -I "${PROTO_DIR}"
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
            ${PROTO_FILE}
        DEPENDS "${PROTO_DIR}/${PROTO_FILE}"
        WORKING_DIRECTORY "${PROTO_DIR}"
        VERBATIM
    )
    
    # Add to the list of generated sources
    list(APPEND GENERATED_SOURCES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc"
    )
endforeach()

# Copy config file to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
    ${CMAKE_CURRENT_BINARY_DIR}/config.yaml
    COPYONLY
)

# Configure version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/version.h
)

# Server executable
add_executable(zonal_controller
    src/server_main.cpp
    src/services/obd_service.cpp
    src/services/lighting_service.cpp
    src/hardware/fuel_level_sensor.cpp
    src/hardware/body_lights.cpp
    ${GENERATED_SOURCES}
    src/config.cpp
)

# Set resource limits for the executable
if(UNIX)
    set_target_properties(zonal_controller PROPERTIES
        LINK_FLAGS "-Wl,-z,now -Wl,-z,relro"
    )
endif()

target_link_libraries(zonal_controller
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LIBRARIES}
    yaml-cpp
    pthread
)

# Install configuration file
install(FILES config.yaml DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/zonal_controller)

# Install version header
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/version.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/zonal_controller
)

# Add version information to the executable
set_target_properties(zonal_controller PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

