/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 1.2

import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0

Item {
    anchors.fill:   parent

    FactPanelController { id: controller; }

    property Fact _battNumber:    controller.getParameterFact(-1, "SPIRIT_BATT_NUM")
    property Fact _payloadWeight: controller.getParameterFact(-1, "SPIRIT_PAYLD_WT")
    property Fact _camType:    controller.getParameterFact(-1, "SPIRIT_CAM_TYPE", false)

    property var cameraModels: [ qsTr("None"), qsTr("Q10F"), qsTr("Q10T"), qsTr("Z10TIR") , qsTr("Z40K") , qsTr("Z40TIR") ]
    Column {
        anchors.fill:       parent

        VehicleSummaryRow {
            labelText: qsTr("Number of batteries")
            valueText: _battNumber.valueString
        }

        VehicleSummaryRow {
            labelText: qsTr("Camera type")
            valueText: cameraModels[_camType.value]
        }

        VehicleSummaryRow {
            labelText: qsTr("Additional payload weight")
            valueText: _payloadWeight.valueString + " lb"
        }
    }
}
