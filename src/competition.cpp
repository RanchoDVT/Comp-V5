#include "vex.h"

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handler for L1 button press on controller.
 */
static void controllerL1Pressed()
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
static void controllerL2Pressed()
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
static void controllerR1Pressed()
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
static void controllerR2Pressed()
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

	drivercontrollogo = 2;
	vex::thread drivepid(drivePID);

	resetDriveSensors = true;
	desiredValue = 28;
	desiredTurnValue = 0;

	vex::this_thread::sleep_for(2000);
	resetDriveSensors = true;
	desiredValue = -28;
	desiredTurnValue = 0;

	vex::this_thread::sleep_for(2000);
	resetDriveSensors = true;
	desiredValue = 28;
	desiredTurnValue = 0;

	vex::this_thread::sleep_for(2000);
	resetDriveSensors = true;
	desiredValue = -28;
	desiredTurnValue = 90;

	ArmMotor.spinFor(2, vex::timeUnits::sec);
	return;
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief User control task.
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
	Drivetrain.setStopping(vex::brakeType::coast);

	while (CONTROLLER1COMMAND)
	{
		double turnVolts = Controller1.Axis1.position() * 0.12; // -12 to 12
		double forwardVolts = Controller1.Axis3.position() * 0.12 * (1 - (std::abs(turnVolts) / 12) * 0.5);
		LeftDriveSmart.spin(vex::directionType::fwd, forwardVolts + turnVolts, vex::voltageUnits::volt);
		RightDriveSmart.spin(vex::directionType::fwd, forwardVolts - turnVolts, vex::voltageUnits::volt);
		vex::this_thread::sleep_for(POLLINGRATE);
	}
	return;
}