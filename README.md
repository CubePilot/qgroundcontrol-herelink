# Herelink QGroundControl Ground Control Station

*QGroundControl* (QGC) is an intuitive and powerful ground control station (GCS) for UAVs.

The primary goal of QGC is ease of use for both first time and professional users. 
It provides full flight control and mission planning for any MAVLink enabled drone, and vehicle setup for both PX4 and ArduPilot powered UAVs. Instructions for *using QGroundControl* are provided in the [User Manual](https://docs.qgroundcontrol.com/en/) (you may not need them because the UI is very intuitive!)

All the code is open-source, so you can contribute and evolve it as you want. 
The [Developer Guide](https://dev.qgroundcontrol.com/en/) explains how to [build](https://dev.qgroundcontrol.com/en/getting_started/) and extend QGC.


Key Links: 
* [Website](http://qgroundcontrol.com) (qgroundcontrol.com)
* [User Manual](https://docs.qgroundcontrol.com/en/)
* [Developer Guide](https://dev.qgroundcontrol.com/en/)
* [Discussion/Support](https://docs.qgroundcontrol.com/en/Support/Support.html)
* [Contributing](https://dev.qgroundcontrol.com/en/contribute/)
* [License](https://github.com/mavlink/qgroundcontrol/blob/master/COPYING.md)

**Build on Herelink:**

To build this repository for Herelink Android System, use following tools:
* [QtCreator](https://www.qt.io/product)
* [Qt 5.11.0](https://qt.io)
* [Android NDK, Revision 15c (July 2017)](https://developer.android.com/ndk/downloads/older_releases)

**Note**: *Qt 5.11.0 is available under archive repository when using online installer for qt installation. It is critical that you use combination above as its tested working. Also ensure that you select armeabi-v7a build of QgroundControl. This repository. Please post your issues at https://docs.cubepilot.org if you are facing issues building this repository.*
