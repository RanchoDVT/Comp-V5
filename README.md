# Advanced Startup Vex V5

## Getting started: ▶️

1: Install VScode  
2: Download and install these extensions:  
([VEX Robotics](https://marketplace.visualstudio.com/items?itemName=VEXRobotics.vexcode), [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools))  
3: Install my [Custom SDK](https://github.com/RanchoDVT/Vex-SDK)

Happy coding! =)  

## Rewrite (3.0)

Here we go! This is a rewritten version of Comp_V2 (with some exceptions).  
We are at b2, I don't Vex V5 Hardware to test with, as school is over.  
When high school starts for me, I *will* be in a robotics class, and then I can start testing.

3.0b3:

1:  Automatic Emergency Braking 🔃 (Requires vision sensor or AI vision)  
(Requires rewrite of User_Control | Prevent acceleration when a collision is imminent)  

- Forward warning - 2 Seconds till impact  
- Caution - 1.5 Seconds till impact  
- Emergency braking - Less than 1 second till impact  

2:  Read advanced motor config and triport config. ✅  
3:  Scrolling on loghandler when messages are too big ✅  
4:  Traction control & Stability control 🔃 (Requires rewrite of User_Control)  
5:  Custom theming 🔃 (Requires rewrite of startup, and needs brain setup)  
6:  Controller settings 🔃 (Requires rewrite of User_Control)  
7:  AWD/FWD/RWD Switcher 🔃 (Requires rewrite of User_Control)  
8:  Odometer 🔃 (Requires tamper protection)  
9:  Maintenance reminders 🔃  
10:  Custom user messages 🔃 (Requires rewrite of startup)  
11:  More debug when connected to a computer 🔃  
12:  PID for autonomous 🔃  
