/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.11
import QtQuick.Controls             2.4
import QtQml.Models                 2.1

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Vehicle       1.0

Item {
    id: checklistItem
    property var model: listModel

    PreFlightCheckModel {
        id:     listModel
        PreFlightCheckGroup {
            name: qsTr("Setup checklist")

            PreFlightCheckButton {
                name:           qsTr("INSPECT ASSEMBLY")
                manualText:     qsTr("All modules and payloads secured")
            }

            PreFlightCheckButton {
                name:           qsTr("CLOCK PROPELLERS")
                manualText:     qsTr("Verify propeller blades' position to prevent entanglement")
            }

            PreFlightCheckButton {
                name:           qsTr("KNOW UAV ORIENTATION")
                manualText:     qsTr("Flashing LEDs point back")
            }
        }

        PreFlightCheckGroup {
            name: qsTr("Configuration checklist")

            PreFlightCheckButton {
                name:            qsTr("SPIRIT SETTINGS")
                manualText:      qsTr("Spirit configuration settings are accurate (see configuration screen)")
            }

            PreFlightCheckButton {
                name:            qsTr("BATTERY ON TOP")
                manualText:      qsTr("Confirm at least ONE battery mounted on top")
            }
        }

        PreFlightCheckGroup {
            name: qsTr("Mission checklist")

            PreFlightCheckButton {
                name:           qsTr("WEATHER")
                manualText:     qsTr("Check wind and weather within limits")
            }

            PreFlightCheckButton {
                name:           qsTr("SAFETY SETTINGS")
                manualText:     qsTr("Review settings in Safety tab")
            }

            PreFlightCheckButton {
                name:           qsTr("MISSION")
                manualText:     qsTr("If applicable - Waypoints and altitudes set correctly")
            }

        }


        PreFlightCheckGroup {
            name: qsTr("Takeoff checklist")

            PreFlightCheckButton {
                name:           qsTr("CHECK SURROUNDINGS")
                manualText:     qsTr("Clear area of obstructions and people")
            }

            PreFlightCheckButton {
                name:           qsTr("HEADING ACCURATE")
                manualText:     qsTr("Reported heading is correct (+-10 deg)")
            }

            PreFlightCheckButton {
                name:           qsTr("CONTROLLER BATTERY")
                manualText:     qsTr("HereLink controller charged")
            }
        
            PreFlightCheckButton {
                name:           qsTr("FLIGHT MODE")
                manualText:     qsTr("Current flight mode set as expected")
            }

            PreFlightCheckButton {
                name:           qsTr("VEHICLE READY")
                manualText:     qsTr("LEDs flash green")
            }
        }
    }
}


