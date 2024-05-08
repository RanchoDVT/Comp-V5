# Advanced Startup Vex V5

## Getting started: ▶️

1: Install VScode  
2: Download and install these extensions:  
([VEX Robotics](https://marketplace.visualstudio.com/items?itemName=VEXRobotics.vexcode), [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools))  
And open the project and happy coding!

## Next version: ⏭️

    2.0pr4:

    1. Ability to create folders to SD card, as you have to have the folders already there or else it can’t create the file…

    2. Custom motor configs with config file. (I hope this is possible!)

    3. Have time holding and more than one button pressed for ctrl1BttnPressed

    4. Improve getUserOption proformance and readabilty.

## Why we use voltage: ❔

1: It dramatically reduces latency. It bypasses PID and lets you have finer controls. "PID is internally calculated at a 10 millisecond rate."[Vex V5 link](https://example.com)  
2: No performance limiting / ready to stop. If you use velocity, the motor wont hit top speed. If you told the motor to be at 100% velocity, it will be at 198 RPM rather than 200. Voltage overcomes this, and forces the performance you want.  
3: It limits torque to 50% of what it can do. So even if it *can* hit 1.1NM of torque before stall, it will make it 0.55. We don't want this especially on a claw or arm.  
4: Fixes the random, all motors turn off and back on.  
