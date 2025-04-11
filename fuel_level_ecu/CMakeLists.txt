cmake_minimum_required(VERSION 3.10)
project(FuelSensorECU)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(fuel_sensor_ecu
    fuel_level_sensor.cpp
    example_usage.cpp
)
