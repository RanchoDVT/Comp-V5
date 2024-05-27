#include "vex.h"

int _stat(
   const char *path,
   struct _stat *buffer
);

/**
 * @brief Reset or initialize the config file based on user input.
 * @param resetreadme Flag to reset readme file.
 * @param message Message to display to the user.
 */
static void resetOrInitializeConfig(const bool &resetreadme, const std::string &message)
{
	std::filesystem::create_directory("sandbox/1/2/b");
	MAXOPTIONSSIZE = 4;
	std::string resetcfg = getUserOption(message, {"Yes", "No"});
	if (resetcfg == "Yes")
	{
		Controller1.Screen.print("Reseting config file...");

		if (resetreadme)
		{
			std::ofstream readmefile("readme.md");
			if (!readmefile)
			{
				logHandler("resetOrInitializeConfig", "Could not create readme.", Log::Level::Warn);
				return;
			}
			readmefile.close();
		}
		else
		{
			std::ofstream configFile("config/config.cfg");
			if (!configFile)
			{
				logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn);
				return;
			}
			// Write default configuration to the file
			configFile << ";Config File:"
					   << "\n";
			configFile << "POLLINGRATE=1"
					   << "\n";
			configFile << "PRINTLOGO=true"
					   << "\n";
			configFile << "CTRLR2ENABLE=false"
					   << "\n";
			configFile << "LOGTOFILE=true"
					   << "\n";
			configFile << "VISIONENABLE=false"
					   << "\n";
			configFile << "MAXOPTIONSSIZE=4"
					   << "\n";
			configFile << "CTRLR1POLLINGRATE=25"
					   << "\n";
			configFile << "LOCALLOGO=false"
					   << "\n";
			configFile << "VERSION=" << VERSION << "\n";
			configFile.close();
		}

		logHandler("resetConfig", "Successfully reset config file.", Log::Level::Debug);
		return;
	}
	else
	{
		logHandler("resetConfig", "Skipped reseting config file.", Log::Level::Debug);
		return;
	}
}

/**
 * @brief Convert a string representation of boolean to bool.
 * @param string String representation of boolean.
 * @return bool Converted boolean value.
 */
bool stringtobool(const std::string &string)
{
	if (string.starts_with("True") or string.starts_with("On") or string.starts_with("true") or string.starts_with("1") or string.starts_with("on"))
	{
		return true;
	}
	else if (string.starts_with("False") or string.starts_with("false") or string.starts_with("off") or string.starts_with("0") or string.starts_with("Off"))
	{
		return false;
	}
	else
	{
		std::ostringstream message;
		message << "Expected bool val. Received: " << string;
		resetOrInitializeConfig(false, message.str());
		return false;
	}
}

/**
 * @brief Convert a string to a l.
 * @param string String to l.
 * @return string Converted to l value.
 */
long stringtol(const std::string &string)
{
	long value;
	

		value = std::stol(string);
		return value;
		std::ostringstream message;
		message << "Expected float val. Received: " << string;
		resetOrInitializeConfig(false, message.str());
		value = std::stol(string.c_str());
		return value;
}

/**
 * @brief Parse and set values from the config file.
 */
static void setValForConfig()
{
	std::ifstream configFile("config/config.cfg");
	if (!configFile)
	{
		logHandler("setValForConfig", "Could not open config file. Reason Unknown.", Log::Level::Warn);
		return;
	}

	std::string line;
	while (std::getline(configFile, line))
	{
		// Parse each line from config file
		if (line.empty() or line[0] == ';' or line[0] == '#')
		{
			continue; // Skip empty lines and comments
		}

		std::istringstream iss(line);
		std::string key, value;
		if (std::getline(iss, key, '=') && std::getline(iss, value))
		{
			if (key == "PRINTLOGO")
			{
				PRINTLOGO = stringtobool(value);
			}
			else if (key == "LOCALLOGO")
			{
				LOCALLOGO = stringtobool(value);
			}
			else if (key == "VISIONENABLE")
			{
				VISIONENABLE = stringtobool(value);
			}
			else if (key == "CTRLR2ENABLE")
			{
				CTRLR2ENABLE = stringtobool(value);
			}
			else if (key == "LOGTOFILE")
			{
				LOGTOFILE = stringtobool(value);
			}
			else if (key == "MAXOPTIONSSIZE")
			{
				MAXOPTIONSSIZE = stringtol(value);
			}
			else if (key == "POLLINGRATE")
			{
				POLLINGRATE = stringtol(value);
			}
			else if (key == "CTRLR1POLLINGRATE")
			{
				CTRLR1POLLINGRATE = stringtol(value);
			}
			else if (key == "VERSION")
			{
				if (value != VERSION)
				{
					std::ostringstream message;
					message << "Version mismatch with Config file (" << value << ") and code version (" << VERSION << "). Do you want to reset the config file?";
					resetOrInitializeConfig(false, message.str());
				}
			}

			else
			{
				std::ostringstream message;
				message << "Unknown key in config file: " << key << "Do you want to reset the config?";
				logHandler("setValForConfig", message.str(), Log::Level::Warn);
				resetOrInitializeConfig(false, message.str());
			}
		}
		else
		{
			std::ostringstream message;
			message << "Invalid line in config file: " << line << "Do you want to reset the config?";
			resetOrInitializeConfig(false, message.str());
		}
	}
	configFile.close();
}

/**
 * @brief Parse config file and initialize variables.
 */
void configParser()
{
	std::ostringstream message;
	message << "Version: " << VERSION << " | Build date: " << BUILD_DATE;
	logHandler("main", message.str(), Log::Level::Info);
	clearScreen(true, true, true);
	Controller1.Screen.print("Starting up...");
	drivercontrollogo = 0;
	if (Brain.SDcard.isInserted())
	{
		if (Brain.SDcard.exists("config/config.cfg"))
		{
			setValForConfig();
		}
		else
		{
			std::filesystem::create_directory("/config");
			resetOrInitializeConfig(false, "Missing config file. Create it?");
			setValForConfig();
			if (!Brain.SDcard.exists("readme.txt"))
			{
				resetOrInitializeConfig(true, "Missing readme. Create it?");
			}
		}
	}
	else
	{
		POLLINGRATE = 1;
		PRINTLOGO = false;
		CTRLR2ENABLE = false;
		VISIONENABLE = false;
		MAXOPTIONSSIZE = 4;
		CTRLR1POLLINGRATE = 25;
		LOCALLOGO = true;
		logHandler("configParser", "No SD card installed. Using default values.", Log::Level::Info);
	}
	calibrategiro();
	Drivetrain.setStopping(vex::brakeType::coast);
	ClawMotor.setStopping(vex::brakeType::coast);
	return;
}
