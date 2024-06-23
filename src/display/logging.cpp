#include "vex.h"

#include <fstream>

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handles and displays error messages, with optional output to the controller's screen and colored output to the console.
 * @param module The module name.
 * @param message The error message.
 * @param level Set the error Level from `Log::Level`.
 */
void logHandler(const std::string &funtionName, const std::string &message, const Log::Level level)
{
    std::ofstream LogFile("log.txt", std::ios_base::out | std::ios_base::app);
    std::string logEntry;
    if (ConfigManager.getLogToFile())
    {
        if (!LogFile)
        {
            logHandler("logHandler", "Could not create logfile.", Log::Level::Warn);
            ConfigManager.setLogToFile(false);
        }
        LogFile << "[" << LogToString(level) << "] > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << funtionName << " > " << message << "\n";
    }

    switch (level)
    {
    case Log::Level::Trace:
    {
        logEntry = "\033[92m[Trace]";
        break;
    }

    case Log::Level::Debug:
    {
        logEntry = "\033[93m[Debug]";
        break;
    }

    case Log::Level::Info:
    {
        logEntry = "\033[94m[Info] ";
        break;
    }

    case Log::Level::Warn:
    {
        logEntry = "\033[38;5;216m[Warn] ";
        clearScreen(false, true, true);
        primaryController.Screen.print(message.c_str());
        primaryController.Screen.newLine();
        primaryController.Screen.print("Check logs.");
        primaryController.Screen.newLine();
        primaryController.Screen.print(("Module: " + funtionName).c_str());
        partnerController.Screen.print(message.c_str());
        partnerController.Screen.newLine();
        partnerController.Screen.print("Check logs.");
        partnerController.Screen.newLine();
        partnerController.Screen.print(("Module: " + funtionName).c_str());
        wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
        clearScreen(false, true, true);
        break;
    }

    case Log::Level::Error:
    {
        logEntry = "\033[31m[Error]";
        clearScreen(false, true, true);
        primaryController.Screen.print(message.c_str());
        primaryController.Screen.newLine();
        primaryController.Screen.print("Check logs.");
        primaryController.Screen.newLine();
        primaryController.Screen.print(("Module: " + funtionName).c_str());
        partnerController.Screen.print(message.c_str());
        partnerController.Screen.newLine();
        partnerController.Screen.print("Check logs.");
        partnerController.Screen.newLine();
        partnerController.Screen.print(("Module: " + funtionName).c_str());
        wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
        clearScreen(false, true, true);
        break;
    }

    case Log::Level::Fatal:
    {
        clearScreen(false, true, true);
        primaryController.Screen.print(message.c_str());
        primaryController.Screen.newLine();
        primaryController.Screen.print("Check logs.");
        primaryController.Screen.newLine();
        primaryController.Screen.print(("Module: " + funtionName).c_str());
        partnerController.Screen.print(message.c_str());
        partnerController.Screen.newLine();
        partnerController.Screen.print("Check logs.");
        partnerController.Screen.newLine();
        partnerController.Screen.print(("Module: " + funtionName).c_str());
        wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
        clearScreen(false, true, true);
        printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", logEntry.c_str(), Brain.Timer.time(vex::timeUnits::sec), funtionName.c_str(), message.c_str());
        LogFile << logEntry << " > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << funtionName << " > " << message << "\n";
        wait(2, vex::seconds);
        vexSystemExitRequest(); // Exit program
        wait(10, vex::seconds);
    }
    }

    printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", logEntry.c_str(), Brain.Timer.time(vex::timeUnits::sec), funtionName.c_str(), message.c_str());
    return;
}