#include "vex.h"

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handler for L1 button press on controller.
 */
void controllerL1Pressed()
{
	ClawMotor.spin(vex::directionType::fwd, 12, vex::voltageUnits::volt);
	while (Controller1.ButtonL1.pressing())
	{
		vex::this_thread::sleep_for(POLLINGRATE);
	}
	ClawMotor.stop();
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handler for L2 button press on controller.
 */
void controllerL2Pressed()
{
	ClawMotor.spin(vex::directionType::rev, 12, vex::voltageUnits::volt);
	while (Controller1.ButtonL2.pressing())
	{
		vex::this_thread::sleep_for(POLLINGRATE);
	}
	ClawMotor.stop();
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handler for R1 button press on controller.
 */
void controllerR1Pressed()
{
	ArmMotor.spin(vex::directionType::rev, ARMVOLTAGE, vex::voltageUnits::volt);
	while (Controller1.ButtonR1.pressing())
	{
		vex::this_thread::sleep_for(POLLINGRATE);
	}
	ArmMotor.stop();
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handler for R2 button press on controller.
 */
void controllerR2Pressed()
{
	ArmMotor.spin(vex::directionType::fwd, ARMVOLTAGE, vex::voltageUnits::volt);
	while (Controller1.ButtonR2.pressing())
	{
		vex::this_thread::sleep_for(POLLINGRATE);
	}
	ArmMotor.stop();
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Autonomous task.
 * @todo Have a timer to end the program if it is at 15 seconds. Need help.
 */
void autonomous()
{
	Drivetrain.setDriveVelocity(60, vex::percentUnits::pct); // Best speed for stability.
	Drivetrain.setTurnVelocity(60, vex::percentUnits::pct);  // Best speed for stability.
	Drivetrain.setStopping(vex::brake);			   // To make sure robot stops in same place every time.
	drivercontrollogo = 2;
	Drivetrain.driveFor(vex::directionType::rev, 20.0, vex::distanceUnits::in, true);
	Drivetrain.driveFor(vex::directionType::fwd, 20.0, vex::distanceUnits::in, true); // Estimated time to finish ~3 seconds.
	vex::this_thread::sleep_for(2000);
	Drivetrain.driveFor(vex::directionType::rev, 20.0, vex::distanceUnits::in, true);
	Drivetrain.driveFor(vex::directionType::fwd, 20.0, vex::distanceUnits::in, true); // Estimated current time ~8 seconds.
	ArmMotor.spinFor(2, vex::seconds);
	return;
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief User control task.
 * @return 0
 */
void userControl()
{
	if (!CONTROLLER1COMMAND)
	{
		logHandler("drivetrain_main", "Ctrl1 is NOT in command mode!", Log::Level::Fatal);
	}

	Controller1.ButtonL1.pressed(controllerL1Pressed);
	Controller1.ButtonL2.pressed(controllerL2Pressed);
	Controller1.ButtonR1.pressed(controllerR1Pressed);
	Controller1.ButtonR2.pressed(controllerR2Pressed);
	vex::this_thread::sleep_for(30); // Wait for callbacks to load.

	vex::thread motortemp(motorTempMonitor);
	// Variables
	float turnVolts;
	float forwardVolts;

	while (CONTROLLER1COMMAND)
	{
		turnVolts = Controller1.Axis1.position() * 0.12; // -12 to 12
		forwardVolts = Controller1.Axis3.position() * 0.12 * (1 - (std::abs(turnVolts) / 12) * 0.5);
		LeftDriveSmart.spin(vex::directionType::fwd, forwardVolts + turnVolts, vex::voltageUnits::volt);
		RightDriveSmart.spin(vex::directionType::fwd, forwardVolts - turnVolts, vex::voltageUnits::volt);
		vex::this_thread::sleep_for(POLLINGRATE);
	}
	return;
}
