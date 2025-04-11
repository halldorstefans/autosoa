cmake_minimum_required(VERSION 3.10)
project(OBD_gRPC_Service)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages using pkg-config (more reliable on Ubuntu/Pop OS)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROTOBUF REQUIRED protobuf)
pkg_check_modules(GRPC REQUIRED grpc++)

# Find protoc and grpc_cpp_plugin executables
find_program(PROTOBUF_PROTOC_EXECUTABLE protoc)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

if(NOT PROTOBUF_PROTOC_EXECUTABLE OR NOT GRPC_CPP_PLUGIN_EXECUTABLE)
    message(FATAL_ERROR "Could not find protoc or grpc_cpp_plugin executables")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
)

# Define proto folder and relative file path
set(PROTO_DIR "${CMAKE_SOURCE_DIR}/../proto")
set(PROTO_FILE "obd_service.proto")

# Generate protobuf and gRPC files
add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/obd_service.pb.cc"
        "${CMAKE_CURRENT_BINARY_DIR}/obd_service.pb.h"
        "${CMAKE_CURRENT_BINARY_DIR}/obd_service.grpc.pb.cc"
        "${CMAKE_CURRENT_BINARY_DIR}/obd_service.grpc.pb.h"
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

# Add generated source files
set(GENERATED_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/obd_service.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/obd_service.grpc.pb.cc"
)

# Server executable
add_executable(obd_server
    src/obd_server_main.cpp
    src/obd_service_impl.cpp
    src/fuel_level_sensor.cpp
    ${GENERATED_SOURCES}
)

target_link_libraries(obd_server
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LIBRARIES}
    pthread
)

# Client executable
add_executable(obd_client
    src/obd_client.cpp
    ${GENERATED_SOURCES}
)

target_link_libraries(obd_client
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LIBRARIES}
    pthread
)
