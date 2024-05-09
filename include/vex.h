#pragma once

#include <string>	 ///< Required for using string objects
#include <sstream>	 ///< Required for using stringstream objects
#include <vector>	 ///< Required for using vector containers
#include <cmath>	 ///< Required for using std::abs with an int
#include <fstream>	 ///< Requried for using std::getline, and making and reading files.
#include <algorithm> ///< Requried for using ::isdigit and std::any_of
#include <filesystem> ///< Requried for using ::isdigit and std::any_of

#include <stdlib.h>	 ///< Required for standard library definitions
#include <stdbool.h> ///< Required for standard boolean definitions
#include <math.h>	 ///< Required for mathematical functions
#include <string.h>	 ///< Required for string manipulation functions
#include <stdio.h>	 ///< Required for standard input/output definitions

#include "v5_cpp.h" ///< Required for VEX V5 definitions

#include "gifclass.h" ///< For vex::Gif class.

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Class for `Log`.
 * @public enum class `Level`.
 */
class Log
{
public:
	enum class Level
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};
};

extern bool CONTROLLER1COMMAND;
extern float POLLINGRATE;
extern bool PRINTLOGO;
extern bool LOCALLOGO;
extern bool VISIONENABLE;
extern bool CTRLR2ENABLE;
extern bool BETAENABLED;
extern bool LOGTOFILE;
extern const std::string &VERSION;
extern const std::string &BUILD_DATE;
extern std::size_t MAXOPTIONSSIZE;
extern std::size_t CTRLR1POLLINGRATE;
extern std::size_t ARMVOLTAGE;
extern int drivercontrollogo;

extern vex::brain Brain;
extern vex::motor LeftDriveSmart;
extern vex::motor RightDriveSmart;
extern vex::inertial Inertial;
extern vex::smartdrive Drivetrain;
extern vex::motor ClawMotor;
extern vex::motor ArmMotor;
extern vex::bumper RearBumper;
extern vex::controller Controller1;
extern vex::controller Controller2;
extern vex::vision::signature PURPLECUBE;
extern vex::vision::signature GREENCUBE;
extern vex::vision::signature ORANGECUBE;
extern vex::vision Vision7;

void calibrategiro();
void configParser();
const char *LogToString(const Log::Level &str);
void clearScreen(const bool &brainClear, const bool &controller1Clear, const bool &controller2Clear);
void logHandler(const std::string &module, const std::string &message, const Log::Level &level);
std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options);
void motorTempMonitor();
void gifplayer();
void userControl();
void autonomous();
void drivePID();

extern int desiredValue;
extern int desiredTurnValue;

extern bool resetDriveSensors;
extern bool enableDrivePID;