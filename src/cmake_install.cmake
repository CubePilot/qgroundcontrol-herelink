# Install script for directory: /home/bendraper/ascentdev/qgroundcontrol-herelink/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/ui/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/ADSB/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Airmap/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/AnalyzeView/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/api/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Audio/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/AutoPilotPlugins/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Camera/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/comm/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/FactSystem/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/FirmwarePlugin/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/FlightDisplay/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/FlightMap/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/FollowMe/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Geo/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/GPS/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Joystick/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/MissionManager/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/PlanView/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/PositionManager/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/QmlControls/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/QtLocationPlugin/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Settings/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Terrain/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/uas/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/Vehicle/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/VehicleSetup/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/VideoStreaming/cmake_install.cmake")
  include("/home/bendraper/ascentdev/qgroundcontrol-herelink/src/ViewWidgets/cmake_install.cmake")

endif()

