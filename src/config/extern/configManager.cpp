#include "vex.h"

#include <fstream>
#include <sstream>

configManager ConfigManager("config/config.cfg", "maintenance.cfg");

// Constructor
configManager::configManager(const std::string &configFileName, const std::string &maintenanceFileName)
    : configFileName(configFileName), maintenanceFileName(maintenanceFileName), maxOptionSize(4), logToFile(true),
      POLLINGRATE(1), PRINTLOGO(true), CTRLR2ENABLE(false), VISIONENABLE(false),
      CTRLR1POLLINGRATE(25), LOCALLOGO(false), odometer(0), lastService(0), serviceInterval(1000)
{
    setValuesFromConfig();
    readMaintenanceData();
}
// Setters with validation
void configManager::setMaxOptionSize(size_t value)
{
    maxOptionSize = value;
}

void configManager::setLogToFile(bool value)
{
    logToFile = value;
}

void configManager::setPollingRate(size_t value)
{
    POLLINGRATE = value;
}

void configManager::setPrintLogo(bool value)
{
    PRINTLOGO = value;
}

void configManager::setCtrlr2Enabled(bool value)
{
    CTRLR2ENABLE = value;
}

void configManager::setVisionEnabled(bool value)
{
    VISIONENABLE = value;
}

void configManager::setCtrlr1PollingRate(std::size_t value)
{
    logToFile = value;
}

void configManager::setLocalLogo(bool value)
{
    LOCALLOGO = value;
}

int configManager::getMotorPort(const std::string &motorName) const
{
    auto it = motorPorts.find(motorName);
    return (it != motorPorts.end()) ? it->second : -1; // Default invalid port
}

std::string configManager::getGearRatio(const std::string &motorName) const
{
    auto it = gearRatios.find(motorName);
    return (it != gearRatios.end()) ? it->second : "6_1"; // Default ratio
}

bool configManager::getMotorReversed(const std::string &motorName) const
{
    auto it = motorReversed.find(motorName);
    return (it != motorReversed.end()) ? it->second : false; // Default reversed state
}

vex::gearSetting configManager::getGearSetting(const std::string &ratio) const
{
    if (ratio == "6_1")
    {
        return vex::gearSetting::ratio6_1;
    }
    else if (ratio == "18_1")
    {
        return vex::gearSetting::ratio18_1;
    }
    else if (ratio == "36_1")
    {
        return vex::gearSetting::ratio36_1;
    }
    else
    {
        return vex::gearSetting::ratio6_1; // Default
    }
}

void configManager::readMaintenanceData()
{
    std::ifstream maintenanceFile(maintenanceFileName);
    if (maintenanceFile.is_open())
    {
        std::string line;
        while (std::getline(maintenanceFile, line))
        {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value))
            {
                if (key == "ODOMETER")
                {
                    odometer = std::stoi(value);
                }
                else if (key == "LAST_SERVICE")
                {
                    lastService = std::stoi(value);
                }
                else if (key == "SERVICE_INTERVAL")
                {
                    serviceInterval = std::stoi(value);
                }
            }
        }
        maintenanceFile.close();
    }
}

void configManager::writeMaintenanceData()
{
    std::ofstream maintenanceFile(maintenanceFileName);
    if (maintenanceFile.is_open())
    {
        maintenanceFile << "ODOMETER=" << odometer << "\n";
        maintenanceFile << "LAST_SERVICE=" << lastService << "\n";
        maintenanceFile << "SERVICE_INTERVAL=" << serviceInterval << "\n";
        maintenanceFile.close();
    }
}

void configManager::updateOdometer(int averagePosition)
{
    odometer += averagePosition;
    if (odometer - lastService >= serviceInterval)
    {
        Brain.Screen.printAt(10, 40, "Service Required! Odometer: %d", odometer);
        lastService = odometer;
    }
    writeMaintenanceData();
}

void configManager::checkServiceInterval()
{
    if (odometer - lastService >= serviceInterval)
    {
        Brain.Screen.printAt(10, 40, "Service Required! Odometer: %d", odometer);
    }
}