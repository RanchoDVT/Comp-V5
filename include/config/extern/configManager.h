#include <map>

class configManager
{
public:
    configManager(const std::string &configFileName, const std::string &maintenanceFileName);
    void resetOrInitializeConfig(bool resetReadme, const std::string &message);
    bool stringToBool(const std::string &str);
    long stringToLong(const std::string &str);
    void setValuesFromConfig();
    void parseConfig();

    std::size_t getMaxOptionSize() const { return maxOptionSize; }
    bool getLogToFile() const { return logToFile; }
    std::size_t getPollingRate() const { return POLLINGRATE; }
    bool getPrintLogo() const { return PRINTLOGO; }
    bool getCtrlr2Enabled() const { return CTRLR2ENABLE; }
    bool getVisionEnabled() const { return VISIONENABLE; }
    std::size_t getCtrlr1PollingRate() const { return CTRLR1POLLINGRATE; }
    bool getLocalLogo() const { return LOCALLOGO; }

    void setMaxOptionSize(size_t value);
    void setLogToFile(bool value);
    void setPollingRate(std::size_t value);
    void setPrintLogo(bool value);
    void setCtrlr2Enabled(bool value);
    void setVisionEnabled(bool value);
    void setCtrlr1PollingRate(std::size_t value);
    void setLocalLogo(bool value);

    std::string getGearRatio(const std::string &motorName) const;
    bool getMotorReversed(const std::string &motorName) const;
    vex::gearSetting getGearSetting(const std::string &ratio) const;
    int getMotorPort(const std::string &motorName) const;

    int getOdometer() const { return odometer; }
    int getLastService() const { return lastService; }
    int getServiceInterval() const { return serviceInterval; }
    void updateOdometer(int averagePosition);
    void checkServiceInterval();

private:
    std::map<std::string, int> motorPorts;
    std::map<std::string, std::string> gearRatios;
    std::map<std::string, bool> motorReversed;
    std::string configFileName;
    std::string maintenanceFileName;
    std::size_t maxOptionSize;
    bool logToFile;
    std::size_t POLLINGRATE;
    bool PRINTLOGO;
    bool CTRLR2ENABLE;
    bool VISIONENABLE;
    std::size_t CTRLR1POLLINGRATE;
    bool LOCALLOGO;

    int odometer;
    int lastService;
    int serviceInterval;

    void readMaintenanceData();
    void writeMaintenanceData();
};