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

The tray icon, desktop widget and keyboard lights for notifications are optional for every alarm. The keyboard lights are only available for setups with Logitech RGB Gaming keyboards and only the first row of keys are used (F1 to Pause keys). When multiple alarms are active the lights of the keyboard will alternate with the state of each alarm. The desktop widget is always on top but it's transparent to user interaction (you can click through it to the windows below).

There is no limit in the number of alarms or their state, all can be running at the same time with their own different notification options and alarm sounds. 

If you like this project you can support it on [Ko-fi](https://ko-fi.com/felixdelaspozas).

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
* [Qt Library](http://www.qt.io/).
* [Logitech Gaming SDK](https://www.logitechg.com/es-es/developers)

# Install
MultiAlarm is available for Windows 10 onwards. You can download the latest installer from the [releases page](https://github.com/FelixdelasPozas/MultiAlarm/releases). Neither the application or the installer are digitally signed so the system will ask for approval before running it the first time.

The last version compatible with windows 7 and 8 is version 1.2.0, you can download it from [here](https://github.com/FelixdelasPozas/MultiAlarm/releases/tag/1.2.0).

# Screenshots
Main dialog with the alarm list. Each alarm has buttons to start, pause, delete and reconfigure it. Clock alarms can't be paused, stopped or reconfigured. When an alarm is running the background pattern of the alarm also fills according to its progression.

![maindialog](https://github.com/user-attachments/assets/09759fed-07aa-472e-869d-f02e63ec105d)

The alarm creation and modification dialog. Keyboard lights option is only active once a Logitech keyboard has been detected.

![newalarmdialog](https://github.com/user-attachments/assets/483d8085-7c23-4148-8877-c7b881a5e261)

Simple alarm completion dialog with the alarm message appears on top of other windows.

![completed](https://github.com/user-attachments/assets/fb17be00-d9ab-4093-a3dc-53b0211582f4)

The tray icon shows the progression of the alarm in eight intervals (doesn't update the progress in real time) and the tooltip show the remaining time and the percentage of passed time. If it's a timer alarm the icon provides the options to manage the alarm without the need of opening the main window.  

![alarmtray](https://github.com/user-attachments/assets/8c979e5e-da9f-4f46-a355-a3e38ec1ac65)

The desktop widget updates in realtime with a pie representation of the remaining time. 

![desktopwidget](https://cloud.githubusercontent.com/assets/12167134/11453849/86e0e7c2-961b-11e5-8ec7-be96f572ea9a.jpg)

Keyboard notification for an alarm with red foreground and white background at 40%. Only the keys from F1 to Pause are used to show the progress of the alarm. If there are multiple keyboard notifications the lights will alternate between alarms. 

![keyboard](https://user-images.githubusercontent.com/12167134/48899357-b1589600-ee4f-11e8-89c4-2d13083acd61.jpg)

# Repository information

**Version**: 1.4.0

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code  |
|:-----------------------------|--------------:|------------:|-----------------:|-----:|
| C++                          |   9           | 473         |   299            | 1696 |
| C/C++ Header                 |   8           | 222         |   667            |  383 |
| CMake                        |   1           |  18         |     8            |   68 |
| **Total**                    | **18**        | **714**     | **974**          | **2147** |
