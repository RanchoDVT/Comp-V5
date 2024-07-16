#include "vex.h"

#include <fstream>

// Method to reset or initialize the config file
void configManager::resetOrInitializeConfig(const std::string &message)
{
    maxOptionSize = 4;
    std::string resetcfg = getUserOption(message, {"Yes", "No"});
    if (resetcfg == "Yes")
    {
        primaryController.Screen.print("Reseting config file...");

        std::ofstream configFile(configFileName);
        if (!configFile)
        {
            logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn, 3);
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

// Method to convert a string to boolean
bool configManager::stringToBool(const std::string &str)
{
    try
    {
        bool boolval = std::stoi(str);
        return boolval;
    }
    catch (const std::invalid_argument &e)
    {
        std::ostringstream message;
        message << "Expected bool value. Received: " << str;
        resetOrInitializeConfig(message.str());
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
        configManager::resetOrInitializeConfig(message.str());
        return 1;
    }
    catch (const std::out_of_range &e)
    {
        std::ostringstream message;
        message << "Val is too large to fit in long! Received: " << str;
        configManager::resetOrInitializeConfig(message.str());
        return 1;
    }
}

// Method to set values from the config file
void configManager::setValuesFromConfig()
{
    std::ifstream configFile(configFileName);
    if (!configFile)
    {
        logHandler("setValForConfig", "Could not open config file. Reason Unknown.", Log::Level::Warn, 4);
        return;
    }

    std::string configLine;
    while (std::getline(configFile, configLine))
    {
        if (configLine.empty() or configLine[0] == ';' or configLine[0] == '#')
        {
            continue; // Skip empty lines and comments
        }

        if (configLine == "MOTOR_CONFIG")
        {
            std::getline(configFile, configLine); // Skip the opening brace
            while (std::getline(configFile, configLine) and configLine != "}")
            {
                if (configLine.empty() or configLine[0] == ';' or configLine[0] == '#')
                {
                    continue; // Skip empty lines and comments
                }

                std::string motorName = configLine;
                std::getline(configFile, configLine); // Skip the opening brace

                std::string motorPort, motorGearRatio, motorReversedStr;
                while (std::getline(configFile, configLine) and configLine != "}")
                {
                    std::istringstream iss(configLine);
                    std::string configKey, configValue;
                    if (std::getline(iss, configKey, '=') and std::getline(iss, configValue))
                    {
                        if (configKey == "PORT")
                        {
                            motorPort = configValue;
                        }
                        else if (configKey == "GEAR_RATIO")
                        {
                            motorGearRatio = configValue;
                        }
                        else if (configKey == "REVERSED")
                        {
                            motorReversedStr = configValue;
                        }
                    }
                }

                motorPorts[motorName] = std::stoi(motorPort);
                motorGearRatios[motorName] = motorGearRatio;
                motorReversed[motorName] = stringToBool(motorReversedStr);
            }
        }
        else if (configLine == "TRIPORT_CONFIG")
        {
            std::getline(configFile, configLine); // Skip the opening brace
            while (std::getline(configFile, configLine) and configLine != "}")
            {
                if (configLine.empty() or configLine[0] == ';' or configLine[0] == '#')
                {
                    continue; // Skip empty lines and comments
                }

                std::string triportName = configLine;
                std::getline(configFile, configLine); // Skip the opening brace

                std::string triportPort;
                while (std::getline(configFile, configLine) and configLine != "}")
                {
                    std::istringstream iss(configLine);
                    std::string configKey, configValue;
                    if (std::getline(iss, configKey, '=') and std::getline(iss, configValue))
                    {
                        if (configKey == "PORT")
                        {
                            triportPort = configValue;
                        }
                    }
                }

                if (triportPort == "A")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.A;
                }
                else if (triportPort == "B")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.B;
                }
                else if (triportPort == "C")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.C;
                }
                else if (triportPort == "D")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.D;
                }
                else if (triportPort == "E")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.E;
                }
                else if (triportPort == "F")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.F;
                }
                else if (triportPort == "G")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.G;
                }
                else if (triportPort == "H")
                {
                    triPorts[triportName] = &Brain.ThreeWirePort.H;
                }
            }
            std::istringstream iss(configLine);
            std::string key, value;
            if (std::getline(iss, key, '=') and std::getline(iss, value))
            {
                if (configLine == "PRINTLOGO")
                {
                    setPrintLogo(stringToBool(value));
                }
                else if (configLine == "VISIONENABLE")
                {
                    setVisionEnabled(stringToBool(value));
                }
                else if (configLine == "CTRLR2ENABLE")
                {
                    setCtrlr2Enabled(stringToBool(value));
                }
                else if (configLine == "LOGTOFILE")
                {
                    setLogToFile(stringToBool(value));
                }
                else if (configLine == "MAXOPTIONSSIZE")
                {
                    setMaxOptionSize(stringToLong(value));
                }
                else if (configLine == "POLLINGRATE")
                {
                    setPollingRate(stringToLong(value));
                }
                else if (configLine == "CTRLR1POLLINGRATE")
                {
                    setCtrlr1PollingRate(stringToLong(value));
                }

                else if (configLine == "VERSION")
                {
                    if (value != Version)
                    {
                        std::ostringstream message;
                        message << "Version mismatch with Config file (" << value << ") and code version (" << Version << "). Do you want to reset the config file?";
                        resetOrInitializeConfig(message.str());
                    }
                }
                else
                {
                    std::ostringstream message;
                    message << "Unknown key in config file: " << key << ". Do you want to reset the config?";
                    resetOrInitializeConfig(message.str());
                }
            }
        }
        else
        {
            std::ostringstream message;
            message << "Invalid line in config file: " << configLine << ". Do you want to reset the config?";
            resetOrInitializeConfig(message.str());
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
        if (Brain.SDcard.exists(configFileName.c_str()))
        {
            setValuesFromConfig();
        }
        else
        {
            resetOrInitializeConfig("Missing config file. Create it?");
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