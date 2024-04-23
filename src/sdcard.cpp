#include "vex.h"

// Function to reset or initialize config file
void resetOrInitializeConfig(bool resetreadme)
{
	std::ofstream configFile("config/config.cfg");
	std::ofstream helpfile("readme.md");
	if (!configFile)
	{
		logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn);
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

	Controller1.Screen.print("Config file reset.");
	logHandler("resetConfig", "Successfully reset config file.", Log::Level::Debug);

	configFile.close();
	helpfile.close();
}

bool stringtobool(std::string string)
{
	if (string.find("True") != std::string::npos or string.find("On") != std::string::npos or string.find("true") != std::string::npos or string.find("1") != std::string::npos or string.find("on") != std::string::npos)
	{
		return true;
	}

	else if (string.find("False") != std::string::npos or string.find("false") != std::string::npos or string.find("0") != std::string::npos or string.find("off") != std::string::npos or string.find("Off") != std::string::npos)
	{
		return false;
	}
	else
	{
		resetOrInitializeConfig(false);
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
		resetOrInitializeConfig(false);
		return 1;
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
		if (line.empty() || line[0] == ';')
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
					logHandler("setValForConfig", message.str(), Log::Level::Warn);
					vex::this_thread::sleep_for(1500);
					std::string resetcfg = getUserOption("Reset Config?", {"Yes", "No"});
					if (resetcfg == "Yes")
					{
						Controller1.Screen.print("Reseting config file...");
						resetOrInitializeConfig(false);
						clearScreen(false, true);
						Controller1.Screen.print("Reset config file.");
						vex::this_thread::sleep_for(1500);
						clearScreen(false, true);
					}
					else
					{
						Controller1.Screen.print("Skipped reset of cfg file.");
						vex::this_thread::sleep_for(1500);
						clearScreen(false, true);
					}
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
				message << "Unknown key in config file: " << key;
				logHandler("setValForConfig", message.str(), Log::Level::Warn);
				vex::this_thread::sleep_for(1500);
				std::string resetcfg = getUserOption("Reset Config?", {"Yes", "No"});
				if (resetcfg == "Yes")
				{
					resetOrInitializeConfig(false);
				}
				else
				{
					Controller1.Screen.print("Skipped reset of cfg file.");
					vex::this_thread::sleep_for(1500);
					clearScreen(false, true);
				}
			}
		}
		else
		{
			std::ostringstream message;
			message << "Invalid line in config file: " << line;
			logHandler("setValForConfig", message.str(), Log::Level::Warn);
			std::string resetcfg = getUserOption("Reset Config?", {"Yes", "No"});
			if (resetcfg == "Yes")
			{
				resetOrInitializeConfig(false);
			}
			else
			{
				Controller1.Screen.print("Skipped reset of cfg file.");
				vex::this_thread::sleep_for(1500);
				clearScreen(false, true);
			}
		}
	}
	configFile.close();
}

// Function to parse config file and initialize variables
void configParser()
{
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
				resetOrInitializeConfig(false);
				setValForConfig();
			}
			else
			{
				resetOrInitializeConfig(true);
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
}