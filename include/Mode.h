#ifndef MODE_H
#define MODE_H

/**
 * @enum SystemMode
 * @brief Enumerates the operational states of the system.
 *
 * This enum defines the various modes in which the system can operate.
 * It is used globally to coordinate behavior across modules such as
 * motor control, fatigue detection, and UI updates.
 */
enum class SystemMode
{
    Normal,           // Default operating mode; manual or non-fatigue mode
    FatigueDetection, // Fatigue detection mode using real-time video processing
    Temp,             // emporary transition mode between states
};

/**
 * @brief Global system mode variable.
 *
 * This external variable reflects the current state of the system.
 */
extern SystemMode g_systemMode;

#endif