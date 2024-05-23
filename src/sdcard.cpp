#include "vex.h"

/**
 * @brief Reset or initialize the config file based on user input.
 * @param resetreadme Flag to reset readme file.
 * @param message Message to display to the user.
 */
static void resetOrInitializeConfig(const bool &resetreadme, const std::string &message)
{
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
	if (string.find("True") or string.find("On") or string.find("true") or string.find("1") or string.find("on"))
	{
		return true;
	}
	else if (string.find("False") or string.find("false") or string.find("off") or string.find("0") or string.find("Off"))
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
 * @brief Convert a string to a ull.
 * @param string String of from ui to ull.
 * @return string Converted to ull value.
 */
long stringtol(const std::string &string)
{
	long value;
	if (std::any_of(string.begin(), string.end(), ::isdigit))
	{
		value = std::atol(string.c_str());
		return value;
	}
	else
	{
		std::ostringstream message;
		message << "Expected float val. Received: " << string;
		resetOrInitializeConfig(false, message.str());
		value = std::atol(string.c_str());
		return value;
	}
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
				bool boolval = stringtobool(value);
				PRINTLOGO = boolval;
			}
			else if (key == "LOCALLOGO")
			{
				bool boolval = stringtobool(value);
				LOCALLOGO = boolval;
			}
			else if (key == "VISIONENABLE")
			{
				bool boolval = stringtobool(value);
				VISIONENABLE = boolval;
			}
			else if (key == "CTRLR2ENABLE")
			{
				bool boolval = stringtobool(value);
				CTRLR2ENABLE = boolval;
			}
			else if (key == "LOGTOFILE")
			{
				bool boolval = stringtobool(value);
				LOGTOFILE = boolval;
			}
			else if (key == "MAXOPTIONSSIZE")
			{
				std::size_t intval = stringtol(value);
				MAXOPTIONSSIZE = intval;
			}
			else if (key == "POLLINGRATE")
			{
				std::size_t intval = stringtol(value);
				POLLINGRATE = intval;
			}
			else if (key == "CTRLR1POLLINGRATE")
			{
				std::size_t intval = stringtol(value);
				CTRLR1POLLINGRATE = intval;
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
	vex::thread gifplay(gifplayer);
	calibrategiro();
	Drivetrain.setStopping(vex::brakeType::coast);
	ClawMotor.setStopping(vex::brakeType::coast);
	return;
}
