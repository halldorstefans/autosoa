#ifndef FUEL_LEVEL_SENSOR_H
#define FUEL_LEVEL_SENSOR_H

/**
 * @brief ECU module for fuel level sensor simulation
 * 
 * This module simulates a fuel level sensor that:
 * - Returns values between 0-100 (percentage)
 * - Includes realistic noise (+/- 2%)
 * - Simulates gradual fuel consumption over time
 * - Allows refueling via an external function
 */
namespace ECU {
    class FuelLevelSensor {
    public:
        /**
         * @brief Constructor
         * @param initial_level Initial fuel level (0-100%)
         * @param consumption_rate Rate of fuel consumption per read (default: 0.01%)
         */
        FuelLevelSensor(float initial_level = 75.0f, float consumption_rate = 0.1f);
        
        /**
         * @brief Read the current fuel level with noise
         * @return Current fuel level (0-100%)
         */
        float read_fuel_level();
        
        /**
         * @brief Add fuel to the tank
         * @param amount Amount to add (0-100%)
         * @return New fuel level
         */
        float refuel(float amount);
        
    private:
        float current_level_;
        float consumption_rate_;
        unsigned int read_count_;
        
        /**
         * @brief Generate noise for sensor reading
         * @return Noise value between -2.0 and 2.0
         */
        float generate_noise();
    };
}

#endif // FUEL_LEVEL_SENSOR_H

