#include "vex.h"

#include <fstream>

// Method to reset or initialize the config file
void configManager::resetOrInitializeConfig(bool resetReadme, const std::string &message)
{
    maxOptionSize = 4;
    std::string resetcfg = getUserOption(message, {"Yes", "No"});
    if (resetcfg == "Yes")
    {
        primaryController.Screen.print("Reseting config file...");

        if (resetReadme)
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
            std::ofstream configFile(configFileName);
            if (!configFile)
            {
                logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn);
                return;
            }
            // Write default configuration to the file
            configFile << ";Config File:\n";
            configFile << "POLLINGRATE=1\n";
            configFile << "PRINTLOGO=true\n";
            configFile << "CTRLR2ENABLE=false\n";
            configFile << "LOGTOFILE=true\n";
            configFile << "VISIONENABLE=false\n";
            configFile << "MAXOPTIONSSIZE=4\n";
            configFile << "CTRLR1POLLINGRATE=25\n";
            configFile << "LOCALLOGO=false\n";
            configFile << "VERSION=" << Version << "\n";
            configFile.close();
        }

        logHandler("resetConfig", "Successfully reset config file.", Log::Level::Debug);
    }
    else
    {
        logHandler("resetConfig", "Skipped reseting config file.", Log::Level::Debug);
    }
}

// Method to convert a string to boolean
bool configManager::stringToBool(const std::string &str)
{
    if (str == "true" || str == "1" || str == "on" || str == "True" || str == "On")
    {
        return true;
    }
    else if (str == "false" || str == "0" || str == "off" || str == "False" || str == "Off")
    {
        return false;
    }
    else
    {
        std::ostringstream message;
        message << "Expected bool value. Received: " << str;
        resetOrInitializeConfig(false, message.str());
        return false;
    }
}

// Method to convert a string to long
long configManager::stringToLong(const std::string &str)
{
    try
    {
        long num = std::stol(str);
        return num;
    }
    catch (const std::invalid_argument &e)
    {
        std::ostringstream message;
        message << "Long val has invalid chars! Received: " << str;
        configManager::resetOrInitializeConfig(false, message.str());
        return 1;
    }
    catch (const std::out_of_range &e)
    {
        std::ostringstream message;
        message << "Val is too large to fit in long! Received: " << str;
        configManager::resetOrInitializeConfig(false, message.str());
        return 1;
    }
}

// Method to set values from the config file
void configManager::setValuesFromConfig()
{
    std::ifstream configFile(configFileName);
    if (!configFile)
    {
        logHandler("setValForConfig", "Could not open config file. Reason Unknown.", Log::Level::Warn);
        return;
    }

    std::string line;
    while (std::getline(configFile, line))
    {
        // Parse each line from config file
        if (line.empty() || line[0] == ';' || line[0] == '#')
        {
            continue; // Skip empty lines and comments
        }

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            if (key == "PORT")
            {
                motorPorts[key] = stringToLong(value);
            }
            else if (key == "GEAR_RATIO")
            {
                gearRatios[key] = value;
            }
            else if (key == "REVERSED")
            {
                motorReversed[key] = stringToBool(value);
            }
            else if (key == "PRINTLOGO")
            {
                setPrintLogo(value.c_str());
            }
            else if (key == "LOCALLOGO")
            {
                setLocalLogo(value.c_str());
                logHandler("configManager::setValuesFromConfig", "Warning! LocalLogo will be deprecated soon! Use "
                                                                 "PRINTLOGO"
                                                                 ".",
                           Log::Level::Warn);
            }
            else if (key == "VISIONENABLE")
            {
                setVisionEnabled(value.c_str());
            }
            else if (key == "CTRLR2ENABLE")
            {
                setCtrlr2Enabled(value.c_str());
            }
            else if (key == "LOGTOFILE")
            {
                setLogToFile(value.c_str());
            }
            else if (key == "MAXOPTIONSSIZE")
            {
                setMaxOptionSize(uintmax_t(value.c_str()));
            }
            else if (key == "POLLINGRATE")
            {
                setPollingRate(uintmax_t(value.c_str()));
            }
            else if (key == "CTRLR1POLLINGRATE")
            {
                setCtrlr1PollingRate(uintmax_t(value.c_str()));
            }
            else if (key == "VERSION")
            {
                if (value != Version)
                {
                    std::ostringstream message;
                    message << "Version mismatch with Config file (" << value << ") and code version (" << Version << "). Do you want to reset the config file?";
                    resetOrInitializeConfig(false, message.str());
                }
            }
            else
            {
                std::ostringstream message;
                message << "Unknown key in config file: " << key << ". Do you want to reset the config?";
                logHandler("setValForConfig", message.str(), Log::Level::Warn);
                resetOrInitializeConfig(false, message.str());
            }
        }
        else
        {
            std::ostringstream message;
            message << "Invalid line in config file: " << line << ". Do you want to reset the config?";
            resetOrInitializeConfig(false, message.str());
        }
    }
    configFile.close();
}

// Method to parse the config file
void configManager::parseConfig()
{
    std::ostringstream message;
    message << "Version: " << Version
            << " | Build date: " << BuildDate;
    logHandler("main", message.str(), Log::Level::Info);
    primaryController.Screen.print("Starting up...");
    if (Brain.SDcard.isInserted())
    {
        if (Brain.SDcard.exists("config/config.cfg"))
        {
            setValuesFromConfig();
        }
        else
        {
            resetOrInitializeConfig(false, "Missing config file. Create it?");
            setValuesFromConfig();
        }
    }
    else
    {
        // Default values
        POLLINGRATE = 1;
        PRINTLOGO = false;
        CTRLR2ENABLE = false;
        VISIONENABLE = false;
        maxOptionSize = 4;
        CTRLR1POLLINGRATE = 25;
        logHandler("configParser", "No SD card installed. Using default values.", Log::Level::Info);
    }
    calibrateGyro();
    gifplayer();
    Drivetrain.setStopping(vex::brakeType::coast);
}