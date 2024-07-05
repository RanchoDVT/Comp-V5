#include "vex.h"

#include <fstream>

// Function to display messages on the controller screens
// #NoRedundantCode
void displayControllerMessage(const std::string &functionName, const std::string &message)
{
    clearScreen(false, true, true);
    primaryController.Screen.print(message.c_str());
    primaryController.Screen.newLine();
    primaryController.Screen.print("Check logs.");
    primaryController.Screen.newLine();
    primaryController.Screen.print(("Module: " + functionName).c_str());
    partnerController.Screen.print(message.c_str());
    partnerController.Screen.newLine();
    partnerController.Screen.print("Check logs.");
    partnerController.Screen.newLine();
    partnerController.Screen.print(("Module: " + functionName).c_str());
    wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
    clearScreen(false, true, true);
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handles and displays error messages, with optional output to the controller's screen and colored output to the console.
 * @param module The module name.
 * @param message The error message.
 * @param level Set the error Level from `Log::Level`.
 * @todo When text is too big to fit on the controller screen, make it scroll.
 */
void logHandler(const std::string &functionName, const std::string &message, const Log::Level level)
{
    std::ofstream LogFile("log.txt", std::ios_base::out | std::ios_base::app);
    if (ConfigManager.getLogToFile())
    {
        if (!LogFile)
        {
            logHandler("logHandler", "Could not create logfile.", Log::Level::Warn);
            ConfigManager.setLogToFile(false);
        }
        LogFile << "[" << LogToString(level) << "] > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << functionName << " > " << message << "\n";
    }
    printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", LogToColor(level), Brain.Timer.time(vex::timeUnits::sec), functionName.c_str(), message.c_str());
    switch (level)
    {
    case Log::Level::Warn:
    case Log::Level::Error:
        displayControllerMessage(functionName, message);
        break;

    case Log::Level::Fatal:
        displayControllerMessage(functionName, message);
        vex::thread::interruptAll();  // Scary! 👾
        vexSystemExitRequest(); // Exit program
        wait(10, vex::seconds); 
        break;

    default:
        break;
    }
    return;
}