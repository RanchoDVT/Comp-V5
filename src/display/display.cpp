#include "vex.h"

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
        logHandler("getUserOption", "Robot is IN competition mode!", Log::Level::Error, 2);
    }

    if (options.size() > ConfigManager.getMaxOptionSize() || options.size() < 2)
    {
        logHandler("getUserOption", "`options` size error!", Log::Level::Error, 2);
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

        int displayedOptions = std::min(static_cast<int>(options.size() - offset), static_cast<int>(buttons.size()));
        for (int i = 0; i < displayedOptions; ++i) // Display available options
        {
            primaryController.Screen.newLine();
            primaryController.Screen.print("%s: %s", buttons[i].c_str(), options[i + offset].c_str());
            buttonString += buttons[i];
            if (i != displayedOptions - 1) // Add comma if not the last button
            {
                buttonString += ", ";
            }
        }

        if (options.size() > buttons.size())
        {
            primaryController.Screen.newLine();
            if (offset + displayedOptions < static_cast<int>(options.size()))
            {
                primaryController.Screen.print(">");
            }
            if (offset > 0)
            {
                primaryController.Screen.print("^");
            }
        }

        optmessage << "Available buttons for current visible options: " << buttonString; // Append button string to message.
        logHandler("getUserOption", optmessage.str(), Log::Level::Debug);
        optmessage.str(std::string());

        const std::string &buttonPressed = std::get<0>(ctrl1BttnPressed());

        auto buttonIt = std::find(buttons.begin(), buttons.end(), buttonPressed);
        if (buttonIt != buttons.end())
        {
            int buttonIndex = std::distance(buttons.begin(), buttonIt);
            if (buttonIndex < displayedOptions)
            {
                Index = buttonIndex + offset;
                logHandler("getUserOption", "[Valid Selection] Index = " + std::to_string(Index) + " | Offset = " + std::to_string(offset), Log::Level::Debug);
                break;
            }
        }
        else if (buttonPressed == "DOWN" && (offset + displayedOptions < static_cast<int>(options.size())))
        {
            ++offset;
            logHandler("getUserOption", "[Scroll DOWN] Offset = " + std::to_string(offset), Log::Level::Debug);
        }
        else if (buttonPressed == "UP" && offset > 0)
        {
            --offset;
            logHandler("getUserOption", "[Scroll UP] Offset = " + std::to_string(offset), Log::Level::Debug);
        }
        else
        {
            logHandler("getUserOption", "[Invalid Selection] Index = " + std::to_string(Index) + " | Offset = " + std::to_string(offset), Log::Level::Debug);
            // Display message
            if (wrongAttemptCount < maxWrongAttempts)
            {
                clearScreen(false, true, true);
                primaryController.Screen.print(wrongMessages[wrongAttemptCount].c_str());
                ++wrongAttemptCount; // Increment wrong attempt count
                logHandler("getUserOption", "wrongAttemptCount: " + std::to_string(wrongAttemptCount), Log::Level::Debug);
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
