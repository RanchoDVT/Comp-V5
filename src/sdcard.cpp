#include "vex.h"

// Function to reset or initialize config file
void resetOrInitializeConfig(bool resetreadme, std::string message)
{
	std::string resetcfg = getUserOption(message.c_str(), {"Yes", "No"});
	if (resetcfg == "Yes")
	{
		Controller1.Screen.print("Reseting config file...");
		std::ofstream configFile("config/config.cfg");
		std::ofstream helpfile("readme.md");
		if (!configFile)
		{
			logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn);
			return;
		}
		if (!helpfile)
		{
			logHandler("resetOrInitializeConfig", "Could not create readme.", Log::Level::Warn);
			return;
		}

		// Write default configuration to the file
		configFile << ";Config File:" << "\n";
		configFile << "POLLINGRATE=1" << "\n";
		configFile << "PRINTLOGO=true" << "\n";
		configFile << "CTRLR2ENABLE=false" << "\n";
		configFile << "VISIONENABLE=false" << "\n";
		configFile << "MAXOPTIONSSIZE=4" << "\n";
		configFile << "CTRLR1POLLINGRATE=25" << "\n";
		configFile << "LOCALLOGO=false" << "\n";
		configFile << "betacode=uF58FlLhU431q28cj599w47Ax5NRi" << "\n";

		logHandler("resetConfig", "Successfully reset config file.", Log::Level::Debug);

		configFile.close();
		helpfile.close();
		return;
	}
	else
	{
		logHandler("resetConfig", "Skipped reseting config file.", Log::Level::Debug);
		return;
	}
}

bool stringtobool(std::string& string)
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

std::size_t stringtofloat(std::string string)
{
	std::size_t value;
	if (std::any_of(string.begin(), string.end(), ::isdigit))
	{
		std::stringstream(string) >> value;
		return value;
	}
	else
	{
		std::ostringstream message;
		message << "Expected float val. Received: " << string;
		resetOrInitializeConfig(false, message.str());
		std::stringstream(string) >> value;
		return value;
	}
}

// Function to parse and set values from config file
void setValForConfig()
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
		if (line.empty() or line[0] == ';')
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
				std::size_t intval = stringtofloat(value);
				MAXOPTIONSSIZE = intval;
			}
			else if (key == "POLLINGRATE")
			{
				std::size_t intval = stringtofloat(value);
				POLLINGRATE = intval;
			}
			else if (key == "CTRLR1POLLINGRATE")
			{
				std::size_t intval = stringtofloat(value);
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
			else if (key == "betacode")
			{
				if (value == "uF58FlLhU431q28cj599w47Ax5NRi")
				{
					BETAENABLED = true;
					logHandler("setValForConfig", "Beta code valid.", Log::Level::Info);
				}
				else
				{
					logHandler("setValForConfig", "Invalid beta code!", Log::Level::Warn);
				}
			}

			else
			{
				std::ostringstream message;
				message << "Unknown key in config file: " << key << "Do you want to reset the config?";
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

// Function to parse config file and initialize variables
void configParser()
{
	std::ostringstream message;
	message << "Version: " << VERSION << " | Build date: " << BUILD_DATE;
	logHandler("main", message.str(), Log::Level::Info);

	if (Brain.SDcard.isInserted())
	{
		if (Brain.SDcard.exists("config/config.cfg"))
		{
			setValForConfig();
		}
		else
		{
			if (Brain.SDcard.exists("readme.md"))
			{
				resetOrInitializeConfig(false, "Missing config file. Create it?");
				setValForConfig();
			}
			else
			{
				resetOrInitializeConfig(true, "Missing readme. Create it?");
				setValForConfig();
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
		LOCALLOGO = false;
		logHandler("configParser", "No SD card installed. Using default values.", Log::Level::Info);
	}
	vex::task calibrate(calibrategiro);
	vex::task gifplay(gifplayer);
	return;
}
