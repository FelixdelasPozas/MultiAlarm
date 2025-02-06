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
Main dialog with the alarm list. Each alarm has buttons to start, pause, delete and reconfigure it. Clock alarms can't be paused, stopped or reconfigured. 

![maindialog](https://cloud.githubusercontent.com/assets/12167134/11453850/8720a5b0-961b-11e5-98c4-c96abe0fc55b.jpg)

The alarm creation and modification dialog with the desktop widget.

![newalarmdialog](https://user-images.githubusercontent.com/12167134/48921204-65810d80-ee9e-11e8-9a94-7f6c389446b6.jpg)

Simple alarm completion dialog with the alarm message appears on top of other windows.

![completed](https://cloud.githubusercontent.com/assets/12167134/11453848/86d73970-961b-11e5-9a2a-ef3af85e7a47.jpg)

The tray icon shows the progression of the alarm in eight intervals (doesn't update the progress in real time) and the tooltip show the remaining time and the percentage of passed time. If it's a timer alarm the icon provides the options to manage the alarm without the need of opening the main window.  

![alarmtray](https://cloud.githubusercontent.com/assets/12167134/11453846/86ce32da-961b-11e5-8e82-32d931176e21.jpg)

The desktop widget updates in realtime with a pie representation of the remaining time. 

![desktopwidget](https://cloud.githubusercontent.com/assets/12167134/11453849/86e0e7c2-961b-11e5-8ec7-be96f572ea9a.jpg)

Application tray icon and menu. 

![apptray](https://cloud.githubusercontent.com/assets/12167134/11766205/4c1a25d2-a17d-11e5-96bc-3d7c6ba5e2a6.jpg)

Keyboard notification for an alarm with red foreground and white background at 40%. Only the keys from F1 to Pause are used to show the progress of the alarm. If there are multiple keyboard notifications the lights will alternate between alarms. 

![keyboard](https://user-images.githubusercontent.com/12167134/48899357-b1589600-ee4f-11e8-89c4-2d13083acd61.jpg)

# Repository information

**Version**: 1.3.2

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code  |
|:-----------------------------|--------------:|------------:|-----------------:|-----:|
| C++                          |   9           | 457         |   296            | 1648 |
| C/C++ Header                 |   8           | 216         |   646            |  360 |
| CMake                        |   1           |  18         |     8            |   68 |
| **Total**                    | **18**        | **691**     | **950**          | **2076** |
