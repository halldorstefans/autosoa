#include "fuel_level_sensor.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace OBD
{
    FuelLevelSensor::FuelLevelSensor(float initial_level, float consumption_rate)
        : current_level_(std::max(0.0f, std::min(100.0f, initial_level))),
          consumption_rate_(consumption_rate),
          read_count_(0)
    {
        // Initialize random seed for noise generation
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    float FuelLevelSensor::read_fuel_level()
    {
        // Simulate fuel consumption over time
        current_level_ = std::max(0.0f, current_level_ - consumption_rate_);

        // Add some noise to the reading
        float noise = generate_noise();

        // Ensure reading stays within valid range (0-100%)
        float reading = std::max(0.0f, std::min(100.0f, current_level_ + noise));

        read_count_++;
        return reading;
    }

    float FuelLevelSensor::refuel(float amount)
    {
        if (amount < 0.0f)
        {
            return current_level_; // Ignore negative refuel amounts
        }

        current_level_ = std::min(100.0f, current_level_ + amount);
        return current_level_;
    }

    float FuelLevelSensor::generate_noise()
    {
        // Generate random noise between -2.0 and 2.0
        return (static_cast<float>(std::rand()) / RAND_MAX * 4.0f) - 2.0f;
    }
}
