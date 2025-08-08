/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.11
import QtQuick.Layouts  1.11

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0
import MAVLink                              1.0

//-------------------------------------------------------------------------
//-- Engine Indicator
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          engineIndicatorRow.width

    property bool showIndicator: true

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    Row {
        id:             engineIndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom

        Loader {
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceComponent:    engineVisual
        }
    }
    
    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, enginePopup)
        }
    }

    Component {
        id: engineVisual

        Row {
            anchors.top:    parent.top
            anchors.bottom: parent.bottom

            function getEngineStatus(){
                var health_status = ['OFF','Stby','Ignite','Acc','Stab','NU','LO','NU','SD','NU','AutoOff','Run','AccDelay','SpdReg','TSR','PreH1','PreH2','NU','NU','FullOn'];
                var stat_idx = _activeVehicle ? _activeVehicle.efi.health.value : 0
                var health_display = health_status[stat_idx] || 'Unknown'
                return qsTr(health_display)
            }

            QGCColoredImage {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                width:              height
                sourceSize.width:   width
                source:             "/qmlimages/Engine.svg"
                fillMode:           Image.PreserveAspectFit
            }

            QGCLabel {
                text:                   getEngineStatus()
                font.pointSize:         ScreenTools.mediumFontPointSize
                color:                  qgcPal.text
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Component {
        id: enginePopup

        Rectangle {
            width:          enginePopupColumn.width + ScreenTools.defaultFontPixelWidth * 3
            height:         enginePopupColumn.height + ScreenTools.defaultFontPixelHeight * 2
            radius:         ScreenTools.defaultFontPixelHeight * 0.5
            color:          qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                     enginePopupColumn
                spacing:                ScreenTools.defaultFontPixelHeight * 0.5
                anchors.margins:        ScreenTools.defaultFontPixelHeight
                anchors.top:            parent.top
                anchors.left:           parent.left

                QGCLabel {
                    text:           qsTr("Engine Status")
                    font.family:    ScreenTools.demiboldFontFamily
                }

                GridLayout {
                    columns: 2
                    columnSpacing: ScreenTools.defaultFontPixelWidth

                    QGCLabel { 
                        text: qsTr("Status:") 
                        Layout.fillWidth: true
                    }
                    QGCLabel { 
                        text: _activeVehicle ? getEngineStatusText() : qsTr("N/A")
                        color: qgcPal.text
                    }

                    QGCLabel { 
                        text: qsTr("RPM:") 
                        Layout.fillWidth: true
                    }
                    QGCLabel { 
                        text: _activeVehicle && _activeVehicle.efi ? _activeVehicle.efi.rpm.valueString : qsTr("N/A")
                        color: qgcPal.text
                    }

                    QGCLabel { 
                        text: qsTr("Gas Temp:") 
                        Layout.fillWidth: true
                    }
                    QGCLabel { 
                        text: _activeVehicle && _activeVehicle.efi ? _activeVehicle.efi.exGasTemp.valueString : qsTr("N/A")
                        color: qgcPal.text
                    }

                    QGCLabel { 
                        text: qsTr("Throttle:") 
                        Layout.fillWidth: true
                    }
                    QGCLabel { 
                        text: _activeVehicle && _activeVehicle.efi ? _activeVehicle.efi.throttlePos.valueString : qsTr("N/A")
                        color: qgcPal.text
                    }

                    QGCLabel { 
                        text: qsTr("Fuel:")
                        Layout.fillWidth: true
                    }
                    QGCLabel { 
                        text: _activeVehicle && _activeVehicle.efi ? _activeVehicle.efi.fuelConsumed.valueString : qsTr("N/A")
                        color: qgcPal.text
                    }
                }

                function getEngineStatusText() {
                    var health_status = ['OFF','Stby','Ignite','Acc','Stab','NU','LO','NU','SD','NU','AutoOff','Run','AccDelay','SpdReg','TSR','PreH1','PreH2','NU','NU','FullOn'];
                    var stat_idx = _activeVehicle.efi.health.value
                    return health_status[stat_idx] || 'Unknown'
                }
            }
        }
    }
}
