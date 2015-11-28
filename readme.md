MultiAlarm
==========

# Summary
- [Description](#description)
- [Compilation](#compilation-requirements)
- [Install](#install)
- [Screenshots](#screenshots)
- [Repository information](#repository-information)

# Description
MultiAlarm is a tool to set multiple timer and clock alarms with system tray and desktop notification widgets. 
Timer alarms can be set to loop, are limited to 24 hours and can be stopped, restarted and reconfigured. Timer alarms are permanently stored until deleted by the user. 
Clock alarms can't be stopped (obviously) and are deleted automatically once they expire, and can be set for any future date and time but not reconfigured. If a clock alarm hasn't expired it starts automatically on every program execution. 
The tray icon and desktop widget for notifications are optional for every alarm. The desktop widget is always on top but it's transparent to user interaction (you can click through it to the windows below).
There is no limit in the number of alarms or their state, all can be running at the same time with their own different notification options and alarm sounds. 

## Options
Several options can be configured:
* different color for each alarm.
* multiple alarm sounds available with volume configuration.
* the opacity of the desktop widget can be configured. 
* the desktop widget can be dragged to any position on the screen, or can be placed in one of the preset positions (multiple monitors are supported). 

# Compilation requirements
## To build the tool:
* cross-platform build system: [CMake](http://www.cmake.org/cmake/resources/software.html).
* compiler: [Mingw64](http://sourceforge.net/projects/mingw-w64/) on Windows.

## External dependencies
The following libraries are required:
* [Qt 5 opensource framework](http://www.qt.io/).

# Install
The only current option is build from source as binaries are not provided. 

# Screenshots
Main dialog with the alarm list. Each alarm has buttons to start, pause, delete and reconfigure it. Clock alarms can't be paused, stopped or reconfigured. 

The alarm creation and modification dialog. 

Simple alarm completion dialog with the alarm message appears on top of other windows.

The tray icon shows the progression of the alarm in eight intervals (doesn't update the progress in real time) and the tooltip show the remaining time and the percentage of passed time. If it's a timer alarm the icon provides the options to manage the alarm without the need of opening the main window.  

The desktop widget updates in realtime with a pie representation of the remaining time. 

Application tray icon and menu. 

# Repository information

**Version**: 1.0.0

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code  |
|:-----------------------------|--------------:|------------:|-----------------:|-----:|
| C++                          |   7           | 385         |   237            | 1472 |
| C/C++ Header                 |   6           | 173         |   508            |  284 |
| CMake                        |   1           |  17         |    13            |   59 |
| **Total**                    | **14**        | **575**     | **758**          | **1815** |
