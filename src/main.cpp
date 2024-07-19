// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    1, 10, 11, 20, IG
// InertialGyro         inertial      2
// Radio                radio         6
// RearBumper           bumper        ThreeWirePort.A
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

std::string Version = "3.0b2 [DEV]";
std::string BuildDate = "7/18/24";

int main()
{
    printf("\033[2J\033[1;1H\033[0m"); // Clears console and Sets color to grey.
    ConfigManager.parseConfig();
    vexCodeInit();
    Competition.autonomous(autonomous);
    Competition.drivercontrol(userControl);
}
