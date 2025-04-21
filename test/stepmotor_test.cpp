#include <iostream>
#include <string>
#include <cstdlib> 
#include "Stepmotor_setting.h"

using namespace std;

/*
 * Usage:
 *   ./motor_control <steps> <direction>
 *
 * Parameters:
 *   steps     - Number of steps the motor should move (e.g., 100)
 *   direction - Movement direction: either "forward" or "backward"
 *
 * Example:
 *   ./motor_control 200 forward
 */

int main(int argc, char* argv[])
{
    // Check if the user provided enough arguments
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <steps> <direction>" << endl;
        return 1;
    }
    int steps = atoi(argv[1]);
    string direction = argv[2];
    StepperMotor motor;
    if(!motor.start(0, 17, 25, 27, 22))
    {
        cerr << "Failed to initialize motor." << endl;
        return 1;
    }

    if (direction == "forward") {
        motor.forward(steps);
    } else if (direction == "backward") {
        motor.backward(steps);
    } else {
        cerr << "Invalid direction. Use 'forward' or 'backward'." << endl;
        motor.cleanup();
        return 1;
    }
    motor.waitUntilDone();
    motor.cleanup();
    cout << "Motor movement complete. GPIO released." << endl;

    return 0;
}

