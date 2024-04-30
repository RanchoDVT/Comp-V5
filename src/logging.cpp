#include "vex.h"

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Handles and displays error messages, with optional output to the controller's screen and colored output to the console.
 * @param module The module name.
 * @param message The error message.
 * @param level Set the error Level from `Log::Level`.
 */
void logHandler(const std::string &module, const std::string &message, const Log::Level &level)
{
	std::ofstream LogFile("log.txt", std::ios_base::out | std::ios_base::app);
	std::string logEntry;
	if (LOGTOFILE)
	{
		if (!LogFile)
		{
			logHandler("logHandler", "Could not create logfile.", Log::Level::Warn);
			LOGTOFILE = false; // Prevent looping :)
		}
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

		clearScreen(false, true);
		Controller1.Screen.print(message.c_str());
		Controller1.Screen.newLine();
		Controller1.Screen.print("Check logs.");
		Controller1.Screen.newLine();
		Controller1.Screen.print(("Module: " + module).c_str());
		wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
		clearScreen(false, true);
	case Log::Level::Warn:
	{
		logEntry = "\033[38;5;216m[Warn] ";
		break;
	}

	case Log::Level::Error:
	{
		logEntry = "\033[31m[Error]";
		break;
	}

	case Log::Level::Fatal:
	{
		printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", logEntry.c_str(), Brain.Timer.time(vex::timeUnits::sec), module.c_str(), message.c_str());
		LogFile << logEntry.c_str() << " > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << module.c_str() << " > " << message.c_str() << "\n";
		wait(2, vex::seconds);
		vexSystemExitRequest(); // Exit program
		wait(10, vex::seconds);
	}
	}

	printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", logEntry.c_str(), Brain.Timer.time(vex::timeUnits::sec), module.c_str(), message.c_str());
	// Write to log file
	if (LOGTOFILE)
	{
		if (!LogFile)
		{
			logHandler("logHandler", "Could not create logfile.", Log::Level::Warn);
			LOGTOFILE = false; // Prevent looping :)
		};
		LogFile << "[" << LogToString(level) << "] > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << module.c_str() << " > " << message.c_str() << "\n";
	}
	return;
}
