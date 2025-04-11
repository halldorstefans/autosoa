#include "fuel_level_sensor.h"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    // Create fuel level sensor with 80% initial fuel
    ECU::FuelLevelSensor fuel_sensor(80.0f);
    
    // Simulate reading the sensor every second for 10 seconds
    for (int i = 0; i < 10; ++i) {
        float level = fuel_sensor.read_fuel_level();
        std::cout << "Fuel level: " << level << "%" << std::endl;
        
        // Wait for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Simulate refueling
    std::cout << "Refueling..." << std::endl;
    fuel_sensor.refuel(15.0f);
    
    // Read again after refueling
    float level = fuel_sensor.read_fuel_level();
    std::cout << "Fuel level after refueling: " << level << "%" << std::endl;
    
    return 0;
}
