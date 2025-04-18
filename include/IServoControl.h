// include/IServoControl.h
#ifndef ISERVOCONTROL_H
#define ISERVOCONTROL_H

/**
 * @interface IServoControl
 * @brief Interface for servo motor angle control abstraction.
 *
 * This interface defines the contract for any class that wishes to implement
 * angle-based control for a servo motor. It enables decoupling between the
 * logic layer (e.g., MotorControl) and the underlying hardware (e.g., MG90S),
 * allowing for flexibility, mocking in unit tests, and cleaner architecture.
 *
 * Classes implementing this interface should override the setAngle() method
 */
class IServoControl
{
public:
    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup of derived classes when deleted via interface pointer.
     */
    virtual ~IServoControl() = default;

    /**
     * @brief Sets the servo motor to the specified angle.
     *
     * @param angle Target angle in degrees, usually ranging from 0 to 180.
     */
    virtual void setAngle(int angle) = 0;
};

#endif
