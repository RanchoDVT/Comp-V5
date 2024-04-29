#include "vex.h"

const char *LogToString(Log::Level str)
{
	switch (str)
	{
	case Log::Level::Trace:
		return "Trace";
	case Log::Level::Debug:
		return "Debug";
	case Log::Level::Info:
		return "Info";
	case Log::Level::Warn:
		return "Warn";
	case Log::Level::Error:
		return "Error";
	case Log::Level::Fatal:
		return "Fatal";
	}
}

/**
 * @author DVT7125
 * @date 4/10/24
 * @brief Clears the screen of the controller and optionally the brain's screen.
 * @param brainClear Wither to clear the brain (`true`) or not (`false`).
 * @param controller1Clear Wither to clear controller1 (`true`) or not (`false`).
 */
void clearScreen(const bool &brainClear, const bool &controller1Clear)
{
	if (brainClear)
	{
		Brain.Screen.clearScreen();
		Brain.Screen.setCursor(1, 1);
	}
	if (controller1Clear)
	{
		Controller1.Screen.clearScreen();
		Controller1.Screen.setCursor(1, 1);
	}
	logHandler("clearScreen", "Finished clearScreen", Log::Level::Trace);
	return;
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief A module that handles controller button press.
 * @return A string representing the button pressed on the controller.
 */
std::string ctrl1BttnPressed()
{
	if (CONTROLLER1COMMAND)
	{
		logHandler("ctrl1BttnPressed", "Ctrl1 is in command mode!", Log::Level::Error);
	}

	std::string buttonPressed;

	while (!CONTROLLER1COMMAND)
	{
		if (Controller1.ButtonA.pressing())
		{
			buttonPressed = "A";
			break;
		}

		else if (Controller1.ButtonX.pressing())
		{
			buttonPressed = "X";
			break;
		}

		else if (Controller1.ButtonY.pressing())
		{
			buttonPressed = "Y";
			break;
		}

		else if (Controller1.ButtonB.pressing())
		{
			buttonPressed = "B";
			break;
		}

		else if (Controller1.ButtonUp.pressing())
		{
			buttonPressed = "UP";
			break;
		}

		else if (Controller1.ButtonDown.pressing())
		{
			buttonPressed = "DOWN";
			break;
		}

		else if (Controller1.ButtonRight.pressing())
		{
			buttonPressed = "RIGHT";
			break;
		}

		else if (Controller1.ButtonLeft.pressing())
		{
			buttonPressed = "LEFT";
			break;
		}

		else if (Controller1.ButtonL1.pressing())
		{
			buttonPressed = "L1";
			break;
		}

		else if (Controller1.ButtonL2.pressing())
		{
			buttonPressed = "L2";
			break;
		}

		else if (Controller1.ButtonR1.pressing())
		{
			buttonPressed = "R1";
			break;
		}
		else if (Controller1.ButtonR2.pressing())
		{
			buttonPressed = "R2";
			break;
		}

		vex::this_thread::sleep_for(CTRLR1POLLINGRATE);
	}

	std::string message = "Selected button: " + buttonPressed;
	logHandler("ctrl1BttnPressed", message, Log::Level::Debug);
	return buttonPressed;
}

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief Displays options and gets user input.
 * @param settingName Passed setting name.
 * @param options Passed options {"test", "test2"} (From 2 options to 4)
 * @return The user's choice.
 */
std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options)
{
	if (CONTROLLER1COMMAND)
	{
		logHandler("getUserOption", "Controller1 is IN command mode!", Log::Level::Error);
	}

	if (options.size() > MAXOPTIONSSIZE or options.size() < 2)
	{
		logHandler("getUserOption", "`options` size error!", Log::Level::Error);
		return options[1]; // Return default, as you have to have at least one option for it to compile.
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

	while (!CONTROLLER1COMMAND)
	{
		buttonString.clear();
		clearScreen(false, true);
		Controller1.Screen.print("%s", settingName.c_str());

		for (int i = 0; i < 2; ++i) // Checks for option size, and allows for options.
		{
			char button;
			if (offset == -1)
			{
				button = 'X';
				if (i == 1)
				{
					button = 'Y';
				}
			}

			else if (offset == -2)
			{
				button = 'Y';
				if (i == 1)
				{
					button = 'B';
				}
			}

			else
			{
				button = 'A';
				if (i == 1)
				{
					button = 'X';
				}
			}

			Controller1.Screen.newLine();
			Controller1.Screen.print("%c: %s", button, options[i - offset].c_str());

			buttonString += button;
			if (i != 1) // Add comma if not the last button
			{
				buttonString += ", ";
			}
		}

		if ((options.size() == 3 and offset != -1) or (options.size() == 4 and offset != -2))
		{
			Controller1.Screen.setCursor(3, 24);
			Controller1.Screen.print(">");
		}
		else if ((options.size() == 3 and offset == -1) or (options.size() == 4 and offset == -2))
		{
			Controller1.Screen.setCursor(3, 24);
			Controller1.Screen.print("^");
		}
		
		optmessage << "Available buttons for current visible options: " << buttonString; // Append button string to message.
		logHandler("getUserOption", optmessage.str(), Log::Level::Debug);
		optmessage.str(std::string());
		
		const std::string &buttonPressed = ctrl1BttnPressed(); // Get user input
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
		else if (buttonPressed == "DOWN" and options.size() >= 3)
		{
			if ((options.size() == 3 and offset != -1) or (options.size() == 4 and offset != -2))
			{
				--offset;
			}
		}
		// If the wrong button is pressed, its index will = the options size, so I know if they get it wrong.

		if (Index < options.size() or offset < 0 or (buttonPressed == "UP" and options.size() >= 3 and offset != 0))
		{
			IndexMessage << "[Valid Selection] Index = " << Index << " | Offset = " << offset; // Append int to string
			logHandler("get_User_Option", IndexMessage.str(), Log::Level::Debug);
			if (buttonPressed == "UP" and offset != 0)
			{
				++offset;
			}
			if (Index < options.size())
			{
				break;
			}
		}

		else
		{
			IndexMessage << "[Invalid Selection] Index = " << Index << " | Offset = " << offset; // Append int to string
			logHandler("getUserOption", IndexMessage.str(), Log::Level::Debug);
			// Display message
			if (wrongAttemptCount < maxWrongAttempts)
			{
				clearScreen(false, true);
				Controller1.Screen.print(wrongMessages[wrongAttemptCount].c_str());
				++wrongAttemptCount; // Increment wrong attempt count
				std::ostringstream failattemptdebug;
				failattemptdebug << "wrongAttemptCount: " << wrongAttemptCount; // Append int to string
				logHandler("getUserOption", failattemptdebug.str(), Log::Level::Debug);
				vex::this_thread::sleep_for(2000);
			}

			else
			{
				logHandler("getUserOption", "Your half arsed.", Log::Level::Fatal);
			}
			optmessage.str(std::string());
		}
	}
	clearScreen(false, true);
	return options[Index];
}

int motorTempMonitor()
{
	logHandler("motorTempMonitor", "motorTempMonitor is starting up...", Log::Level::Trace);
	int clawTemp;
	int armTemp;
	int leftDriveTemp;
	int rightDriveTemp;
	std::ostringstream motorTemps;
	std::ostringstream dataBuffer;

	while (CONTROLLER1COMMAND)
	{
		motorTemps.str(std::string());
		dataBuffer.str(std::string());

		clawTemp = ClawMotor.temperature(vex::temperatureUnits::celsius);
		armTemp = ArmMotor.temperature(vex::temperatureUnits::celsius);
		leftDriveTemp = LeftDriveSmart.temperature(vex::temperatureUnits::celsius);
		rightDriveTemp = RightDriveSmart.temperature(vex::temperatureUnits::celsius);

		// Check for overheat conditions for each motor
		if (clawTemp >= 60)
		{
			motorTemps << "CM overheat: " << clawTemp << "°";
			logHandler("motorTempMonitor", motorTemps.str(), Log::Level::Warn);
		}
		if (armTemp >= 60)
		{
			motorTemps << "AM overheat: " << armTemp << "°";
			logHandler("motorTempMonitor", motorTemps.str(), Log::Level::Warn);
		}
		if (leftDriveTemp >= 60)
		{
			motorTemps << "LM overheat: " << leftDriveTemp << "°";
			logHandler("motorTempMonitor", motorTemps.str(), Log::Level::Warn);
		}
		if (rightDriveTemp >= 60)
		{
			motorTemps << "RM overheat: " << rightDriveTemp << "°";
			logHandler("motorTempMonitor", motorTemps.str(), Log::Level::Warn);
		}
		// Log motor temperatures
		motorTemps << "\n | LeftTemp: " << leftDriveTemp << "°\n | RightTemp: " << rightDriveTemp << "°\n | ArmTemp: " << armTemp << "°\n | ClawTemp: " << clawTemp << "°\n | Battery Voltage: " << Brain.Battery.voltage() << "V\n";
		logHandler("motorTempMonitor", motorTemps.str(), Log::Level::Info);
		dataBuffer << "\nX Axis: " << Inertial.acceleration(vex::axisType::xaxis) << "\nY Axis: " << Inertial.acceleration(vex::axisType::yaxis) << "\nZ Axis: " << Inertial.acceleration(vex::axisType::zaxis);
		logHandler("motorTempMonitor", dataBuffer.str(), Log::Level::Info);
		clearScreen(false, true);
		Controller1.Screen.print("LM: %d° | RM: %d°", leftDriveTemp, rightDriveTemp);
		Controller1.Screen.newLine();
		Controller1.Screen.print("AM: %d° | CM: %d°", armTemp, clawTemp);
		Controller1.Screen.newLine();
		Controller1.Screen.print("Battery: %.1fV", Brain.Battery.voltage());
		vex::this_thread::sleep_for(5000);
	}
	return 0;
}

int gifplayer()
{
	if (drivercontrollogo == 1)
	{
		vex::Gif gif("assets/drivercontrol.gif", 0, 0, true);

		while (LOCALLOGO)
		{
			Brain.Screen.printAt(5, 230, "frame %3d", gif.getFrameIndex());
		}
	}
	else if (drivercontrollogo == 0)
	{
		vex::Gif gif("assets/autonomous.gif", 0, 0, true);

		while (LOCALLOGO)
		{
			Brain.Screen.printAt(5, 230, "frame %3d", gif.getFrameIndex());
		}
	}
	else
	{
		vex::Gif gif("assets/loading.gif", 0, 0, true);

		while (LOCALLOGO)
		{
			Brain.Screen.printAt(5, 230, "frame %3d", gif.getFrameIndex());
		}
	}
	return 0;
}

int calibrategiro()
{
	logHandler("calibrateDrivetrain", "Calibrating Inertial Gyro...", Log::Level::Info);
	Inertial.calibrate();

	while (Inertial.isCalibrating())
	{
		vex::this_thread::sleep_for(25);
	}
	logHandler("calibrateDrivetrain", "Finished Calibrating Inertial Gyro.", Log::Level::Info);
	return 1;
}
