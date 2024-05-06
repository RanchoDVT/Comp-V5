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

	// Write to log file
	if (LOGTOFILE)
	{
		if (!LogFile)
		{
			LOGTOFILE = false; // Prevent looping :)
			logHandler("logHandler", "Could not create logfile.", Log::Level::Warn);
		};
		if (!(LogFile.badbit == 1L))
		{
			LOGTOFILE = false; // Prevent looping :)
			logHandler("logHandler", "Warning, bad byte detected.", Log::Level::Warn);
		}
		LogFile << "[" << LogToString(level) << "] > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << module << " > " << message << "\n";
		LogFile.close();
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
		clearScreen(false, true);
		Controller1.Screen.print(message.c_str());
		Controller1.Screen.newLine();
		Controller1.Screen.print("Check logs.");
		Controller1.Screen.newLine();
		Controller1.Screen.print(("Module: " + module).c_str());
		wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
		clearScreen(false, true);
		logEntry = "\033[38;5;216m[Warn] ";
		break;
	}

	case Log::Level::Error:
	{
		clearScreen(false, true);
		Controller1.Screen.print(message.c_str());
		Controller1.Screen.newLine();
		Controller1.Screen.print("Check logs.");
		Controller1.Screen.newLine();
		Controller1.Screen.print(("Module: " + module).c_str());
		wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
		clearScreen(false, true);
		logEntry = "\033[31m[Error]";
		break;
	}

	case Log::Level::Fatal:
	{
		clearScreen(false, true);
		Controller1.Screen.print(message.c_str());
		Controller1.Screen.newLine();
		Controller1.Screen.print("Check logs.");
		Controller1.Screen.newLine();
		Controller1.Screen.print(("Module: " + module).c_str());
		wait(2, vex::seconds); // Prevent other tasks from running and let user read message.
		clearScreen(false, true);
		printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", logEntry.c_str(), Brain.Timer.time(vex::timeUnits::sec), module.c_str(), message.c_str());
		wait(2, vex::seconds);
		vexSystemExitRequest(); // Exit program
		wait(10, vex::seconds);
	}
	}

	printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", logEntry.c_str(), Brain.Timer.time(vex::timeUnits::sec), module.c_str(), message.c_str());
	return;
}