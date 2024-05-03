// ---- Start of device config notes ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller
// Drivetrain           smartdrive    1, 10, DrivetrainInertial
// DrivetrainInertial   inertial      2
// ClawMotor            motor         3
// ArmMotor             motor         8
// Radio                radio         6
// RearBumper           bumper        ThreeWirePort.A
// ---- End of device config notes ----

#include "vex.h" /// Header file

std::size_t MAXOPTIONSSIZE;
std::size_t CTRLR1POLLINGRATE;
std::size_t ARMVOLTAGE; // Voltage setting for arm motor.
float POLLINGRATE;		// In MS. | 20ms (Default) = 50Hz | 10ms (PID for motors) = 100Hz | 2ms = 500Hz (10X faster) | 1ms = 1000Hz (20X faster) | 0.125ms = 8000Hz (160X faster)
bool PRINTLOGO;
bool LOCALLOGO;
bool VISIONENABLE;
bool CTRLR2ENABLE;
bool BETAENABLED;
bool LOGTOFILE;
std::string VERSION = "2.0pr2";
std::string BUILD_DATE = "4/25/24";
bool CONTROLLER1COMMAND = false;
int drivercontrollogo;

/* Stall Torque (with 36:1 gears)	2.1 Nm			*/
/* Encoder	1800 ticks/rev with 36:1 gears			*/
/* Stall Torque (with 18:1 gears)	1.1 Nm			*/
/* Encoder	900 ticks/rev with 18:1 gears			*/

vex::brain Brain;
vex::motor LeftDriveSmart = vex::motor(vex::PORT1, vex::ratio36_1, false);
vex::motor RightDriveSmart = vex::motor(vex::PORT10, vex::ratio36_1, true);
vex::inertial Inertial = vex::inertial(vex::PORT2);
vex::smartdrive Drivetrain = vex::smartdrive(LeftDriveSmart, RightDriveSmart, Inertial, 319.19, 320, 40, vex::mm, 1);
vex::motor ClawMotor = vex::motor(vex::PORT3, vex::ratio36_1, false);
vex::motor ArmMotor = vex::motor(vex::PORT8, vex::ratio36_1, false);
vex::bumper RearBumper = vex::bumper(Brain.ThreeWirePort.A);
vex::controller Controller1 = vex::controller(vex::primary);
vex::controller Controller2 = vex::controller(vex::partner);
vex::competition Competition;
/*vex-vision-config:begin*/
vex::vision::signature PURPLECUBE = vex::vision::signature(1, 1755, 3073, 2414, 6847, 9223, 8035, 4.7, 0);
vex::vision::signature GREENCUBE = vex::vision::signature(2, -7943, -4519, -6231, -4025, -2043, -3034, 2.7, 0);
vex::vision::signature ORANGECUBE = vex::vision::signature(3, 8099, 9043, 8571, -1895, -1371, -1633, 2.5, 0);
vex::vision Vision7 = vex::vision(vex::PORT7, 50, PURPLECUBE, GREENCUBE, ORANGECUBE);
/*vex-vision-config:end*/

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Pre-autonomous setup.
 * @bug `brainBattery << "Battery is at: " << (Brain.Battery.capacity())) << "%%";` prints %% to console rather than %
 */
void startup()
{
	clearScreen(true, true);
	drivercontrollogo = 00;

	configParser();

	std::ostringstream message;

	Controller1.Screen.print("Starting up...");
	logHandler("startup", "Starting GUI startup...", Log::Level::Info);

	if (CONTROLLER1COMMAND)
	{
		logHandler("startup", "Ctrl1 is in command mode!", Log::Level::Fatal);
	}

	vex::competition::bStopAllTasksBetweenModes = false;

	message << "Battery is at: " << Brain.Battery.capacity() << "%%";
	if (Brain.Battery.capacity() < 90)
	{
		logHandler("startup", message.str(), Log::Level::Warn);
	}
	else
	{
		logHandler("startup", message.str(), Log::Level::Info);
	}
	message.str(std::string());

	std::string armSetting = getUserOption("Arm Mode:", {"Hold", "Coast"});
	if (armSetting == "Hold")
	{
		ArmMotor.setStopping(vex::hold);
	}
	else if (armSetting == "Coast")
	{
		ArmMotor.setStopping(vex::coast);
	}

	message << "Arm set to " << armSetting << ".";
	logHandler("startup", message.str(), Log::Level::Trace);
	Controller1.Screen.print((message.str().c_str()));
	vex::this_thread::sleep_for(1000);

	message.str(std::string());

	std::string ArmVolts = getUserOption("Arm Volts:", {"9", "6", "12"});
	if (ArmVolts == "9")
	{
		ARMVOLTAGE = 9;
	}
	else if (ArmVolts == "6")
	{
		// Allows for more precise controls
		ARMVOLTAGE = 6;
	}
	else if (ArmVolts == "12")
	{
		ARMVOLTAGE = 12;
	}
	
	message << "Arm set to " << ARMVOLTAGE << " volts.";
	logHandler("startup", message.str(), Log::Level::Trace);
	Controller1.Screen.print((message.str()).c_str());
	message.str(std::string());
	vex::this_thread::sleep_for(1000);

	std::string autorun = getUserOption("Run Autonomous?", {"Yes", "No"});
	if (autorun == "Yes")
	{
		logHandler("startup", "Starting autonomous from setup.", Log::Level::Trace);
		Controller1.Screen.print("Running autonomous.");
		autonomous();
		logHandler("startup", "Finished autonomous.", Log::Level::Trace);
	}
	else if (autorun == "No")
	{
		Controller1.Screen.print("Skipped autonomous.");
		logHandler("startup", "Skipped autonomous.", Log::Level::Trace);
		vex::this_thread::sleep_for(1000);
	}
	CONTROLLER1COMMAND = true;
	drivercontrollogo = 1;
	clearScreen(false, true);
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Main function.
 * @return 0
 */
int main()
{
	printf("\033[2J\033[1;1H\033[0m"); // Clears console and Sets color to grey.
	Competition.autonomous(autonomous);
	startup();
	Competition.drivercontrol(userControl);
}
