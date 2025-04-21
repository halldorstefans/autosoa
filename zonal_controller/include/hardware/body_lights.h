/**
 * @file body_lights.h
 * @brief Hardware abstraction layer for vehicle body lights
 * @author Auto SOA Team
 * @version 1.0.0
 * @date 2024-04-16
 */

#ifndef BODY_LIGHTS_H
#define BODY_LIGHTS_H

namespace Body {

/**
 * @class Lights
 * @brief Hardware abstraction layer for vehicle body lights control
 * 
 * This class provides a hardware abstraction layer for controlling
 * the vehicle's body lights. It currently supports:
 * - Headlight control (on/off)
 * - Headlight state querying
 * 
 * @note This is a simulation class and does not interface with actual hardware
 */
class Lights {
public:
    /**
     * @brief Construct a new Lights object
     * 
     * Initializes all lights to their default state (off)
     */
    Lights();

    /**
     * @brief Set the headlight state
     * 
     * @param state true to turn on, false to turn off
     * @return bool true if operation was successful
     */
    bool set_headlight(bool state);

    /**
     * @brief Get the current headlight state
     * 
     * @return bool true if headlight is on, false if off
     */
    bool get_headlight_state();

private:
    bool is_headlight_on_;  ///< Current state of the headlight
};

} // namespace Body

#endif // BODY_LIGHTS_H 