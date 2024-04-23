
#pragma once

#include <string>	 ///< Required for using string objects
#include <sstream>	 ///< Required for using stringstream objects
#include <vector>	 ///< Required for using vector containers
#include <cmath>	 ///< Required for using std::abs with an int
#include <fstream>	 ///< Requried for using std::getline, and making and reading files.
#include <fstream>	 ///< Requried for using std::getline, and making and reading files.
#include <algorithm> ///< Requried for using ::isdigit and std::any_of

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
extern std::string VERSION;
extern std::string BUILD_DATE;
extern std::size_t MAXOPTIONSSIZE;
extern std::size_t CTRLR1POLLINGRATE;
extern std::size_t ARMVOLTAGE; // Voltage setting for arm motor.

extern vex::brain Brain;
extern vex::motor LeftDriveSmart;
extern vex::motor RightDriveSmart;
extern vex::inertial Inertial;
extern vex::smartdrive Drivetrain;
extern vex::motor ClawMotor;
extern vex::motor ArmMotor;
extern vex::bumper RearBumper;
extern vex::controller Controller1;

void configParser();
void clearScreen(const bool &brainClear, const bool &controller1Clear);
void logHandler(const std::string &module, const std::string &message, const Log::Level &level);
std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options);
int motorTempMonitor();
void userControl();
void autonomous();
const char *LogToString(Log::Level str);
int gifplayer();