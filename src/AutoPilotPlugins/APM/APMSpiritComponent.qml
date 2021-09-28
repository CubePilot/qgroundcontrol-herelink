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
import QtQuick.Dialogs  1.2
import QtQuick.Layouts  1.2

import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0

SetupPage {
    id:             spiritPage
    pageComponent:  spiritPageComponent

    FactPanelController {
        id:         controller
    }

    property real _margins:         ScreenTools.defaultFontPixelHeight

    Component {
        id: spiritPageComponent

        Flow {
            id:         flowLayout
            width:      availableWidth
            spacing:    _margins

            property Fact battNumber:    controller.getParameterFact(-1, "SPIRIT_BATT_NUM", false)
            property Fact payloadWeight: controller.getParameterFact(-1, "SPIRIT_PAYLD_WT", false)
            property Fact camType:    controller.getParameterFact(-1, "SPIRIT_CAM_TYPE", false)

            QGCPalette { id: ggcPal; colorGroupEnabled: true }

            Column {
                spacing: 20
                visible: true

                Rectangle {
                    width:  availableWidth
                    height: spiritParamsGrid.y + spiritParamsGrid.height + _margins
                    color:  ggcPal.windowShade

                    GridLayout {
                        id: spiritParamsGrid
                        columns:        2
                        rowSpacing:     _margins
                        columnSpacing:  _margins
                        anchors.margins:    _margins

                        QGCLabel {
                            Layout.row:     1
                            Layout.column:  0
                            anchors.leftMargin: 10
                            anchors.topMargin: 10
                            text:       qsTr("Spirit parameters")
                            font.family: ScreenTools.demiboldFontFamily
                            font.pointSize: 15
                        }

                        QGCLabel {
                            id:             numberOfBattFieldBanner
                            Layout.row:     2
                            Layout.column:  0
                            anchors.leftMargin: 10
                            anchors.topMargin: 10
                            text:           qsTr("Number of batteries:")
                        }

                        QGCComboBox {
                            id:             numberOfBattField
                            anchors.left:   numberOfBattFieldBanner.right
                            anchors.verticalCenter:   numberOfBattFieldBanner.verticalCenter
                            anchors.leftMargin: 5
                            width:          ScreenTools.defaultFontPixelWidth * 15
                            model:          [ qsTr("1"), qsTr("2") ]
                            currentIndex:   battNumber.value - 1
                            onActivated:    battNumber.value = index + 1
                        }

                        QGCLabel {
                            anchors.left: numberOfBattField.right
                            anchors.verticalCenter: numberOfBattField.verticalCenter
                            anchors.leftMargin: 5
                            text:           qsTr("WARNING - ONE battery shall be mounted on the top of the vehicle")
                        }

                        QGCLabel {
                            id: camTypeFieldBanner
                            Layout.row:     3
                            Layout.column:  0
                            anchors.leftMargin: 10
                            text:           qsTr("Camera type")
                        }

                        QGCComboBox {
                            id:             camTypeField
                            anchors.left: camTypeFieldBanner.right
                            anchors.verticalCenter: camTypeFieldBanner.verticalCenter
                            anchors.leftMargin: 5
                            width:          ScreenTools.defaultFontPixelWidth * 15
                            model:          [ qsTr("None"), qsTr("Q10F"), qsTr("Q10T"), qsTr("Z10TIR-35") , qsTr("Z40K") , qsTr("Z40TIR") ]

                            currentIndex:   camType.value
                            onActivated:    camType.value = index
                        }

                        Text {
                            anchors.left: camTypeField.right
                            anchors.top: camTypeField.bottom
                            anchors.leftMargin: 20
                            font.pointSize: 11
                            textFormat: Text.RichText
                            wrapMode: Text.Wrap
                            text: "<p>Supported cameras</p> <p><ul><li>Q10F - Single Sensor, Visual Light, x10 zoom <li>Q10T - Single Sensor, Visual Light, x10 zoom, Tracking <li>Z10TIR - Dual Sensor, Visual and Thermal, x10 zoom, Tracking <li> Z40K - Single Sensor, Visual Light, x40 zoom, 4k resolution <li>Z40TIR - Dual Sensor, Visual and Thermal, x40 zoom, 4k resolution, Tracking</ul></p>"
                        }
                    }
                }

                Rectangle {
                    width:  availableWidth
                    height: spiritAdvParamsGrid.y + spiritAdvParamsGrid.height + _margins
                    color:  ggcPal.windowShade

                    GridLayout {
                        id: spiritAdvParamsGrid
                        columns:        2
                        rowSpacing:     _margins
                        columnSpacing:  _margins
                        anchors.margins:    _margins

                        QGCCheckBox {
                            id:         advancedUserCheckbox
                            text:       qsTr("ADVANCED PARAMETERS - ENABLE WITH CAUTION")
                            Layout.row:     5
                            Layout.column:  0
                            anchors.leftMargin: 10
                            anchors.topMargin: 10
                            checked:    false
                        }

                        QGCLabel {
                            id:     plWeightFieldBanner
                            Layout.row:     6
                            Layout.column:  0
                            text:           qsTr("Additional Payload weight in lbs:")
                            enabled: advancedUserCheckbox.checked
                        }

                        FactTextField {
                            id:     plWeightField
                            anchors.left: plWeightFieldBanner.right
                            anchors.verticalCenter: plWeightFieldBanner.verticalCenter
                            width:  20
                            anchors.leftMargin: 5
                            anchors.topMargin: 50
                            fact:   payloadWeight
                            enabled: advancedUserCheckbox.checked
                        }

                        Text {
                            anchors.left: plWeightField.right
                            anchors.top: plWeightFieldBanner.top
                            anchors.leftMargin: 5
                            font.pointSize: 11
                            textFormat: Text.RichText
                            wrapMode: Text.Wrap
                            text: "(Excluding landing gear and standard GPS lid)</p>"
                        }
                    }
                }
            } // Column
        } // Flow
    } // Component - spiritPageComponent
} // SetupPage
