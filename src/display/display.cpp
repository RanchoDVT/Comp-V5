#include "vex.h"

/**
 * @author DVT7125
 * @date 4/10/24
 * @brief Clears the screen of the controller and optionally the brain's screen.
 * @param brainClear Wither to clear the brain (`true`) or not (`false`).
 * @param primaryControllerClear Wither to clear primaryController (`true`) or not (`false`).
 * @param partnerControllerClear Wither to clear partnerController (`true`) or not (`false`).
 */
void clearScreen(const bool &brainClear, const bool &primaryControllerClear, const bool &partnerControllerClear)
{
    if (brainClear)
    {
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(1, 1);
    }
    if (primaryControllerClear)
    {
        primaryController.Screen.clearScreen();
        primaryController.Screen.setCursor(1, 1);
    }
    if (partnerControllerClear)
    {
        partnerController.Screen.clearScreen();
        partnerController.Screen.setCursor(1, 1);
    }
    logHandler("clearScreen", "Finished clearScreen", Log::Level::Trace);
    return;
}

std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options)
{
    if (Competition.isEnabled())
    {
        logHandler("getUserOption", "Robot is IN competition mode!", Log::Level::Error);
    }

    if (options.size() > ConfigManager.getMaxOptionSize() or options.size() < 2)
    {
        logHandler("getUserOption", "`options` size error!", Log::Level::Error);
        return "DEFAULT";
    }

    std::ostringstream optmessage;
    optmessage << "Options: ";
    for (auto optionIterator = options.begin(); optionIterator != options.end(); ++optionIterator)
    {
        if (optionIterator != options.begin())
        {
            optmessage << ", "; // Separate options with a comma and space (except for the first option)
        }
        optmessage << *optionIterator;
    }

    logHandler("getUserOption", optmessage.str(), Log::Level::Debug);
    optmessage.str(std::string());

    std::size_t wrongAttemptCount = 0;
    const std::size_t maxWrongAttempts = 3;
    const std::string wrongMessages[maxWrongAttempts] = {
        "Invalid selection!", "Do you need a map?", "Rocks can do this!"};
    std::string buttonString;
    // Have a index number for what they selected and convert it to the option.
    std::size_t Index = options.size(); // Invalid selection by default
    int offset = 0;

    const std::vector<std::string> &buttons = {"A", "X", "Y", "B"};
    const std::vector<std::string> &scrollButtons = {"DOWN", "UP"};

    while (!primaryController.installed())
    {
        vex::this_thread::sleep_for(20);
    }

    while (!Competition.isEnabled() and primaryController.installed())
    {
        buttonString.clear();
        clearScreen(false, true, true);
        primaryController.Screen.print(settingName.c_str());
        partnerController.Screen.print("Waiting for #1...");

        for (int i = 0; i < 2; ++i) // Checks for option size, and allows for options.
        {
            primaryController.Screen.newLine();
            primaryController.Screen.print("%s: %s", buttons[i - offset].c_str(), options[i - offset].c_str());
            buttonString += buttons[i - offset];
            if (i != 1) // Add comma if not the last button
            {
                buttonString += ", ";
            }
        }

        if ((options.size() == 3 and offset != -1) or (options.size() == 4 and offset != -2))
        {
            primaryController.Screen.setCursor(3, 24);
            primaryController.Screen.print(">");
        }
        else if ((options.size() == 3 and offset == -1) or (options.size() == 4 and offset == -2))
        {
            primaryController.Screen.setCursor(3, 24);
            primaryController.Screen.print("^");
        }

        optmessage << "Available buttons for current visible options: " << buttonString; // Append button string to message.
        logHandler("getUserOption", optmessage.str(), Log::Level::Debug);
        optmessage.str(std::string());

        const std::string &buttonPressed = std::get<0>(ctrl1BttnPressed());

        if (buttonPressed == "A")
        {
            Index = 0;
        }
        else if (buttonPressed == "X" and options.size() >= 2)
        {
            Index = 1;
        }
        else if (buttonPressed == "Y" and options.size() >= 3)
        {
            Index = 2;
        }
        else if (buttonPressed == "B" and options.size() == 4)
        {
            Index = 3;
        }

        if (Index < options.size())
        {
            optmessage << "[Valid Selection] Index = " << Index << " | Offset = " << offset;
            logHandler("get_User_Option", optmessage.str(), Log::Level::Debug);
            break;
        }

        else if (end(scrollButtons) != std::find(begin(scrollButtons), end(scrollButtons), buttonPressed))
        {
            if (buttonPressed == "DOWN" and ((options.size() == 3 and offset != -1) or (options.size() == 4 and offset != -2)))
            {
                --offset;
            }
            else if (buttonPressed == "UP" and offset != 0)
            {
                ++offset;
            }
            optmessage << "[Valid Selection] Index = " << Index << " | Offset = " << offset;
            logHandler("get_User_Option", optmessage.str(), Log::Level::Debug);
        }

        else
        {
            optmessage << "[Invalid Selection] Index = " << Index << " | Offset = " << offset;
            logHandler("getUserOption", optmessage.str(), Log::Level::Debug);
            // Display message
            if (wrongAttemptCount < maxWrongAttempts)
            {
                clearScreen(false, true, true);
                primaryController.Screen.print(wrongMessages[wrongAttemptCount].c_str());
                ++wrongAttemptCount; // Increment wrong attempt count
                std::ostringstream failattemptdebug;
                failattemptdebug << "wrongAttemptCount: " << wrongAttemptCount;
                logHandler("getUserOption", failattemptdebug.str(), Log::Level::Debug);
                vex::this_thread::sleep_for(2000);
            }

            else
            {
                logHandler("getUserOption", "Your half arsed.", Log::Level::Fatal);
            }
        }
        optmessage.str(std::string());
    }
    clearScreen(false, true, true);
    return options[Index];
}