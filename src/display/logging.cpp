#include "vex.h"

#include <fstream>

/// @brief *For loghandler*
/// @param str takes Log::Level level
/// @return returns Log::Level code in string.
const char *LogToString(const Log::Level &str)
{
    switch (str)
    {
    case Log::Level::Trace:
        return "Trace";
    case Log::Level::Debug:
        return "Debug";
    case Log::Level::Info:
        return "Info";
    case Log::Level::Warn:
        return "Warn";
    case Log::Level::Error:
        return "Error";
    case Log::Level::Fatal:
        return "Fatal";
    default:
        return "Error";
    }
}

/// @brief *For loghandler*
/// @param str takes Log::Level level
/// @return Color code of Log::Level in string.
const char *LogToColor(const Log::Level &str)
{
    switch (str)
    {
    case Log::Level::Trace:
    {
        return "\033[92m[Trace]";
    }

    case Log::Level::Debug:
    {
        return "\033[93m[Debug]";
    }

    case Log::Level::Info:
    {
        return "\033[94m[Info]";
    }

    case Log::Level::Warn:
    {
        return " \033[38;5;216m[Warn]";
    }

    case Log::Level::Error:
    {
        return "\033[31m[Error]";
    }

    case Log::Level::Fatal:
    {
        return "\033[32m[Fatal]";
    }
    default:
    {
        return "\033[31m[Error]";
    }
    }
}

// Helper function to scroll text on the controller screen
void scrollText(const std::string &text, vex::controller &controller, const float &timeOfDisplay)
{
    int length = text.length();
    int displayLength = 20;
    int startIndex = 0;
    bool forward = true;

    for (float elapsed = 0; elapsed < timeOfDisplay; elapsed += 0.3)
    {
        controller.Screen.setCursor(1, 1);
        controller.Screen.clearLine();
        controller.Screen.print(text.substr(startIndex, displayLength).c_str());

        // Update startIndex for scrolling
        if (forward)
        {
            startIndex++;
            if (startIndex + displayLength >= length)
            {
                forward = false;
            }
        }
        else
        {
            startIndex--;
            if (startIndex <= 0)
            {
                forward = true;
            }
        }

        wait(0.3, vex::timeUnits::sec);
    }
}

// Display messages on controller screens
// #NoRedundantCode
void displayControllerMessage(const std::string &functionName, const std::string &message, const float &timeOfDisplay)
{
    clearScreen(false, true, true);
    if (message.length() > 20)
    {
        scrollText(message, primaryController, timeOfDisplay);
        scrollText(message, partnerController, timeOfDisplay);
        primaryController.Screen.newLine();
        primaryController.Screen.print("Check logs.");
        primaryController.Screen.newLine();
        primaryController.Screen.print(("Module: " + functionName).c_str());
        partnerController.Screen.print(message.c_str());
        partnerController.Screen.newLine();
        partnerController.Screen.print("Check logs.");
        partnerController.Screen.newLine();
        partnerController.Screen.print(("Module: " + functionName).c_str());
        wait(timeOfDisplay, vex::timeUnits::sec); // Prevent other tasks from running and let user read message.
    }
    else
    {
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
        wait(timeOfDisplay, vex::timeUnits::sec); // Prevent other tasks from running and let user read message.
    }
    clearScreen(false, true, true);
}

// Log handler function
void logHandler(const std::string &functionName, const std::string &message, const Log::Level level, const float &timeOfDisplay)
{
    std::ofstream LogFile("log.txt", std::ios_base::out | std::ios_base::app);
    if (ConfigManager.getLogToFile())
    {
        if (!LogFile)
        {
            logHandler("logHandler", "Could not create logfile.", Log::Level::Warn, 3);
            ConfigManager.setLogToFile(false);
        }
        LogFile << "[" << LogToString(level) << "] > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << functionName << " > " << message << "\n";
    }
    printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", LogToColor(level), Brain.Timer.time(vex::timeUnits::sec), functionName.c_str(), message.c_str());

    if (level == Log::Level::Warn || level == Log::Level::Error || level == Log::Level::Fatal)
    {
        displayControllerMessage(functionName, message, timeOfDisplay);
    }

    if (level == Log::Level::Fatal)
    {
        displayControllerMessage(functionName, message, timeOfDisplay);
        vex::thread::interruptAll(); // Scary! 👾
        vexSystemExitRequest();      // Exit program
        wait(10, vex::timeUnits::sec);
    }
}

// Overload logHandler for Trace, Debug, Info without timeOfDisplay
void logHandler(const std::string &functionName, const std::string &message, const Log::Level level)
{
    logHandler(functionName, message, level, 2);
}