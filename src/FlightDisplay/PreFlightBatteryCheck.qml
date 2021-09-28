/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick 2.3

import QGroundControl           1.0
import QGroundControl.Controls  1.0
import QGroundControl.Vehicle   1.0

// This class stores the data and functions of the check list but NOT the GUI (which is handled somewhere else).
PreFlightCheckButton {
    name:                           qsTr("BATTERY LEVEL")
    manualText:                     ""
    telemetryFailure:               _batLow
    telemetryTextFailure:           allowTelemetryFailureOverride ?
                                        qsTr("Warning - Battery Voltage below %1.").arg(failureVoltage) :
                                        qsTr("Battery Voltage below %1 V. Please recharge.").arg(failureVoltage)
    allowTelemetryFailureOverride:  allowFailureVoltageOverride

    property int    failureVoltage:                 43
    property bool   allowFailureVoltageOverride:    false
    property var    _batteryValue:                  activeVehicle ? activeVehicle.battery.voltage.value : 0
    property var    _batVoltageRemaining:           isNaN(_batteryValue) ? 0 : _batteryValue
    property bool   _batLow:                        _batVoltageRemaining < failureVoltage
}
