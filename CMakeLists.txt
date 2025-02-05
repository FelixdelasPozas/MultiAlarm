cmake_minimum_required (VERSION 3.10.0)

project(MultiAlarm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0020 OLD)

# Version Number
set (MULTIALARM_VERSION_MAJOR 1)
set (MULTIALARM_VERSION_MINOR 3)
set (MULTIALARM_VERSION_PATCH 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt6 COMPONENTS Widgets Multimedia)

# Fixed, need to be changed to your own installation of Logitech Gaming LED SKD files. 
set(LOGITECH_INCLUDE "D:/Desarrollo/Code/LogitechG810/include/")
set(LOGITECH_LIBRARY "D:/Desarrollo/Code/LogitechG810/include/LogitechLedEnginesWrapper.a")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Multimedia_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -m64 -Wall -mwindows -Wno-deprecated")

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(CORE_EXTERNAL_LIBS ${CORE_EXTERNAL_LIBS} ${QT_QTTEST_LIBRARY})
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if(DEFINED MINGW)
  configure_file("${PROJECT_SOURCE_DIR}/resources.rc.in" "${PROJECT_BINARY_DIR}/resources.rc")
  configure_file("${PROJECT_SOURCE_DIR}/installer/script.iss.in" "${PROJECT_BINARY_DIR}/script.iss")
  set(CORE_SOURCES ${CORE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/resources.rc)
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff -o <OBJECT> -i <SOURCE>")
  enable_language(RC)
endif(DEFINED MINGW)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}          # Generated .h files
  ${CMAKE_CURRENT_BINARY_DIR}  # For wrap/ui files
  ${LOGITECH_INCLUDE}
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -std=c++11 -mwindows")

# Add Qt Resource files
qt6_add_resources(RESOURCES
  rsc/resources.qrc
)

qt6_wrap_ui(CORE_UI
  # .ui for Qt
  NewAlarmDialog.ui
  MainWindow.ui
  AboutDialog.ui
  AlarmWidget.ui
)
	
set (CORE_SOURCES
  # project files
  ${CORE_SOURCES}
  ${RESOURCES}
  ${CORE_MOC}
  ${CORE_UI}
  Main.cpp
  NewAlarmDialog.cpp
  MultiAlarm.cpp
  AboutDialog.cpp
  AlarmWidget.cpp
  Alarm.cpp
  DesktopWidget.cpp
  LogiLED.cpp
  Utils.cpp
)

set(CORE_EXTERNAL_LIBS
  ${LOGITECH_LIBRARY}
  Qt6::Widgets
  Qt6::Multimedia
)
  
add_executable(MultiAlarm ${CORE_SOURCES})
target_link_libraries (MultiAlarm ${CORE_EXTERNAL_LIBS})
