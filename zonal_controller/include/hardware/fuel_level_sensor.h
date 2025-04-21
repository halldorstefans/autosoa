/**
 * @file fuel_level_sensor.h
 * @brief ECU module for fuel level sensor simulation
 * @author Auto SOA Team
 * @version 1.0.0
 * @date 2024-04-16
 */

#ifndef FUEL_LEVEL_SENSOR_H
#define FUEL_LEVEL_SENSOR_H

namespace OBD {

/**
 * @class FuelLevelSensor
 * @brief Simulates a fuel level sensor with realistic behavior
 * 
 * This class provides a simulation of a fuel level sensor that:
 * - Returns values between 0-100 (percentage)
 * - Includes realistic noise (+/- 2%)
 * - Simulates gradual fuel consumption over time
 * - Allows refueling via an external function
 * 
 * @note This is a simulation class and does not interface with actual hardware
 */
class FuelLevelSensor {
public:
    /**
     * @brief Construct a new Fuel Level Sensor object
     * 
     * @param initial_level Initial fuel level (0-100%)
     * @param consumption_rate Rate of fuel consumption per read (default: 0.01%)
     * @throw std::invalid_argument If initial_level is outside valid range
     */
    FuelLevelSensor(float initial_level = 75.0f, float consumption_rate = 0.1f);
    
    /**
     * @brief Read the current fuel level with noise
     * 
     * This method:
     * 1. Decrements the fuel level by consumption_rate
     * 2. Adds random noise to the reading
     * 3. Ensures the result is within valid range (0-100%)
     * 
     * @return float Current fuel level (0-100%)
     */
    float read_fuel_level();
    
    /**
     * @brief Add fuel to the tank
     * 
     * @param amount Amount to add (0-100%)
     * @return float New fuel level after refueling
     * @throw std::invalid_argument If amount is negative
     */
    float refuel(float amount);
    
private:
    float current_level_;        ///< Current fuel level (0-100%)
    float consumption_rate_;     ///< Rate of fuel consumption per read
    unsigned int read_count_;    ///< Counter for number of reads
    
    /**
     * @brief Generate noise for sensor reading
     * 
     * @return float Noise value between -2.0 and 2.0
     */
    float generate_noise();
};

} // namespace OBD

#endif // FUEL_LEVEL_SENSOR_H 