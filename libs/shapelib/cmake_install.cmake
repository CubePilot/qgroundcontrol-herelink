# Install script for directory: /home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/share/shp/export_shp.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}/usr/local/share/shp/export_shp.cmake"
         "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/CMakeFiles/Export/_usr/local/share/shp/export_shp.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}/usr/local/share/shp/export_shp-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}/usr/local/share/shp/export_shp.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/shp/export_shp.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/share/shp" TYPE FILE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/CMakeFiles/Export/_usr/local/share/shp/export_shp.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/usr/local/share/shp/export_shp-relwithdebinfo.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "/usr/local/share/shp" TYPE FILE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/CMakeFiles/Export/_usr/local/share/shp/export_shp-relwithdebinfo.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libshp.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/libshp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shpcreate" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shpcreate")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/shpcreate"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/shpcreate")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/shpcreate")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shpcreate" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shpcreate")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/shpcreate"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/shpcreate")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shpadd" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shpadd")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/shpadd"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/shpadd")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/shpadd")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shpadd" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shpadd")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/shpadd"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/shpadd")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shpdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shpdump")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/shpdump"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/shpdump")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/shpdump")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shpdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shpdump")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/shpdump"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/shpdump")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shprewind" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shprewind")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/shprewind"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/shprewind")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/shprewind")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shprewind" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shprewind")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/shprewind"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/shprewind")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/dbfcreate" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/dbfcreate")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/dbfcreate"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/dbfcreate")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/dbfcreate")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/dbfcreate" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/dbfcreate")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/dbfcreate"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/dbfcreate")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/dbfadd" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/dbfadd")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/dbfadd"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/dbfadd")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/dbfadd")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/dbfadd" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/dbfadd")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/dbfadd"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/dbfadd")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/dbfdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/dbfdump")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/dbfdump"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/dbfdump")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/dbfdump")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/dbfdump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/dbfdump")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/dbfdump"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/dbfdump")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shptreedump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shptreedump")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/shptreedump"
         RPATH "/usr/local/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/shptreedump")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/shptreedump")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/shptreedump" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/shptreedump")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/shptreedump"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "/usr/local/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/shptreedump")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/shapefil.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include" TYPE FILE FILES "/home/bendraper/ascentdev/qgroundcontrol-herelink/libs/shapelib/shapefil.h")
endif()

