project(MultiAlarm)

cmake_minimum_required (VERSION 2.8.6)

# Version Number
set (MULTIALARM_VERSION_MAJOR 1)
set (MULTIALARM_VERSION_MINOR 0)
set (MULTIALARM_VERSION_PATCH 0)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(CORE_EXTERNAL_LIBS ${CORE_EXTERNAL_LIBS} ${QT_QTTEST_LIBRARY})
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if(DEFINED MULTIALARM_VERSION_MAJOR)
  configure_file("${PROJECT_SOURCE_DIR}/resources.rc.in" "${PROJECT_BINARY_DIR}/resources.rc")
  set(CORE_SOURCES ${CORE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/resources.rc)
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff -o <OBJECT> -i <SOURCE>")
  enable_language(RC)
endif(DEFINED MULTIALARM_VERSION_MAJOR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}          # Generated .h files
  ${CMAKE_CURRENT_BINARY_DIR}  # For wrap/ui files
  )

set(CMAKE_CXX_FLAGS " -Wall -Wno-deprecated -std=c++11 -mwindows")

# Add Qt Resource files
qt4_add_resources(RESOURCES
  rsc/resources.qrc
)

qt4_wrap_ui(CORE_UI
  # .ui for Qt
)
	
qt4_wrap_cpp(CORE_MOC
  # .h for Qt
)

set (CORE_SOURCES
  # project files
  ${CORE_SOURCES}
  ${RESOURCES}
  ${CORE_MOC}
  ${CORE_UI}
  Main.cpp
)

set(CORE_EXTERNAL_LIBS
  ${QT_LIBRARIES}
)
  
add_executable(MultiAlarm ${CORE_SOURCES})
target_link_libraries (MultiAlarm ${CORE_EXTERNAL_LIBS})