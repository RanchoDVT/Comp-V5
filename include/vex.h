#include <stdlib.h>

#include "v5_cpp.h"

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

/// @brief 
extern std::string Version;
/// @brief 
extern std::string BuildDate;

/// @brief Handles logging, to sd card if supported, to user if warn and higher, and outputs it in the console.
/// @param funtionName Name of funtion that you want to log.
/// @param message The message! (Ig you can't understand this, you don't deserve a computer)
/// @param level The log level (from the class Log::Level)
void logHandler(const std::string &funtionName, const std::string &message, const Log::Level level);
const char *LogToString(const Log::Level &str);
/// @brief GUI for settting options
/// @param settingName The name for what you want to change (duh)
/// @param options The options you want (duh), in a vector like this: {"Opt1", "Opt2"}
/// @return a string output of what got selected.
std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options);
/// @brief Helper cmd for clearing screens on one line of code.
/// @param brainClear Weither to clear the screen on the V5 Brain
/// @param primaryControllerClear Weither to clear the screen on the 1st controller
/// @param partnerControllerClear Weither to clear the screen on the 2nd controller
void clearScreen(const bool &brainClear, const bool &primaryControllerClear, const bool &partnerControllerClear);
/// @brief Funtion to calabrate Gyro
void calibrateGyro();
/// @brief Caller for autonomous code
void autonomous();
/// @brief 
void motorTempMonitor();
/// @brief
void userControl();
/// @brief Gets input from the controller, NO GUI.
/// @return Time of button held (int), and what button got pressed (string)
std::pair<std::string, int> ctrl1BttnPressed();

#include "config/robot-config.h"

#include "config/extern/configManager.h"

extern configManager ConfigManager;

#include "display/gifdec.h"