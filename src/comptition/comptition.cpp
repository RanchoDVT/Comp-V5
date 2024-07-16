#include "vex.h"

void autonomous()
{
    logHandler("autonomous", "Test message.", Log::Level::Warn, 2);
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
	if (!Competition.isEnabled())
	{
		logHandler("drivetrain_main", "Ctrl1 is NOT in command mode!", Log::Level::Fatal);
	}

	vex::thread motortemp(motorTempMonitor);

	// Variables
	double turnVolts;
	double forwardVolts;

	while (Competition.isEnabled())
	{
		turnVolts = primaryController.Axis1.position() * 0.12; // -12 to 12
		forwardVolts = primaryController.Axis3.position() * 0.12 * (1 - (std::abs(turnVolts) / 12));
		LeftDriveSmart.spin(vex::forward, forwardVolts + turnVolts, vex::voltageUnits::volt);
		RightDriveSmart.spin(vex::forward, forwardVolts - turnVolts, vex::voltageUnits::volt);
		vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
	}
	return;
}