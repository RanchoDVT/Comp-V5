#include "vex.h"

// Settings
double kP = 0.0;
double kI = 0.0;
double kD = 0.0;
double turnKp = 0.0;
double turnKi = 0.0;
double turnKd = 0.0;
int maxTurnIntegral = 300; // These cap the integrals
int maxIntegral = 300;
int integralBound = 3; // If error is outside the bounds, then apply the integral. This is a buffer with +-integralBound degrees

// Autonomous Settings
int desiredValue = 200;
int desiredTurnValue = 0;

int error;          // SensorValue - DesiredValue : Position
int prevError = 0;  // Position 20 miliseconds ago
int derivative;     // error - prevError : Speed
int totalError = 0; // totalError = totalError + error

int turnError;          // SensorValue - DesiredValue : Position
int turnPrevError = 0;  // Position 20 miliseconds ago
int turnDerivative;     // error - prevError : Speed
int turnTotalError = 0; // totalError = totalError + error

bool resetDriveSensors = false;

// Variables modified for use
bool enableDrivePID = true;

// Pasted from a C++ resource
double signnum_c(double x)
{
    if (x > 0.0)
        return 1.0;
    if (x < 0.0)
        return -1.0;
    return x;
}

int drivePID()
{

    while (enableDrivePID)
    {

        if (resetDriveSensors)
        {
            resetDriveSensors = false;
            LeftDriveSmart.setPosition(0, vex::rotationUnits::deg);
            RightDriveSmart.setPosition(0, vex::rotationUnits::deg);
        }

        // Get the position of both motors
        int leftMotorPosition = LeftDriveSmart.position(vex::rotationUnits::deg);
        int rightMotorPosition = RightDriveSmart.position(vex::rotationUnits::deg);

        ///////////////////////////////////////////
        // Lateral movement PID
        /////////////////////////////////////////////////////////////////////
        // Get average of the two motors
        int averagePosition = (leftMotorPosition + rightMotorPosition) / 2;

        // Potential
        error = averagePosition - desiredValue;

        // Derivative
        derivative = error - prevError;

        // Integral
        if (abs(error) < integralBound)
        {
            totalError += error;
        }
        else
        {
            totalError = 0;
        }
        // totalError += error;

        // This would cap the integral
        totalError = abs(totalError) > maxIntegral ? signnum_c(totalError) * maxIntegral : totalError;

        double lateralMotorPower = error * kP + derivative * kD + totalError * kI;
        /////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        // Turning movement PID
        /////////////////////////////////////////////////////////////////////
        // Get average of the two motors
        int turnDifference = leftMotorPosition - rightMotorPosition;

        // Potential
        turnError = turnDifference - desiredTurnValue;

        // Derivative
        turnDerivative = turnError - turnPrevError;

        // Integral
        if (abs(error) < integralBound)
        {
            turnTotalError += turnError;
        }
        else
        {
            turnTotalError = 0;
        }
        // turnTotalError += turnError;

        // This would cap the integral
        turnTotalError = abs(turnTotalError) > maxIntegral ? signnum_c(turnTotalError) * maxIntegral : turnTotalError;

        double turnMotorPower = turnError * turnKp + turnDerivative * turnKd + turnTotalError * turnKi;
        /////////////////////////////////////////////////////////////////////

        LeftDriveSmart.spin(vex::directionType::fwd, lateralMotorPower + turnMotorPower, vex::voltageUnits::volt);
        RightDriveSmart.spin(vex::directionType::fwd, lateralMotorPower - turnMotorPower, vex::voltageUnits::volt);

        prevError = error;
        turnPrevError = turnError;
        vex::task::sleep(20);
    }

    return 1;
}