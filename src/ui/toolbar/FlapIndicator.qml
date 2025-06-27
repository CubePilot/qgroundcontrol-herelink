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
//-- Flap Indicator
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          flapIndicatorRow.width

    property bool showIndicator: _activeVehicle ? _activeVehicle.rcFlap !== undefined : false

    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property int    _rcFlapValue:       _activeVehicle ? _activeVehicle.rcFlap : 1500
    property bool   _vehicleConnected:  _activeVehicle ? !_activeVehicle.connectionLost : false

    // Flap position thresholds (PWM values)
    readonly property int _flapUpThreshold:     1800    // Above this is UP
    readonly property int _flapDownThreshold:   1200    // Below this is DOWN
    readonly property int _flapMinPWM:          1000    // Minimum PWM value
    readonly property int _flapMaxPWM:          2000    // Maximum PWM value

    Row {
        id:             flapIndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth * 0.25

        Loader {
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceComponent:    _vehicleConnected ? flapVisual : disconnectedVisual
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, flapPopup)
        }
    }

    Component {
        id: flapVisual

        Row {
            anchors.top:    parent.top
            anchors.bottom: parent.bottom
            spacing:        ScreenTools.defaultFontPixelWidth * 0.25

            // Flap status functions
            function getFlapStatus() {
                if (!_activeVehicle || _rcFlapValue === undefined) {
                    return qsTr("N/A")
                }

                if (_rcFlapValue >= _flapUpThreshold) {
                    return qsTr("FLAT")
                } else if (_rcFlapValue <= _flapDownThreshold) {
                    return qsTr("DOWN")
                } else {
                    return qsTr("MID")
                }
            }

            function getFlapColor() {
                if (!_activeVehicle || _rcFlapValue === undefined) {
                    return qgcPal.colorGrey
                }

                if (_rcFlapValue >= _flapUpThreshold) {
                    return qgcPal.colorGreen      // FLAT - Normal flight position
                } else if (_rcFlapValue <= _flapDownThreshold) {
                    return qgcPal.colorOrange     // DOWN - Landing/takeoff position
                } else {
                    return qgcPal.colorBlue       // MID - Intermediate position
                }
            }

            function getFlapPercentage() {
                if (!_activeVehicle || _rcFlapValue === undefined) {
                    return 0
                }
                // Convert PWM to percentage (0-100%)
                var percentage = ((_rcFlapValue - _flapMinPWM) / (_flapMaxPWM - _flapMinPWM)) * 100
                return Math.max(0, Math.min(100, Math.round(percentage)))
            }

            function getFlapValue() {
                var pwm = _rcFlapValue
                return  pwm

            }

            QGCColoredImage {
                id:                     flapIcon
                anchors.top:            parent.top
                anchors.bottom:         parent.bottom
                width:                  height
                sourceSize.width:       width
                source:                 "/qmlimages/Flap.svg"
                fillMode:               Image.PreserveAspectFit
                color:                  getFlapColor()

                // Add subtle animation for status changes
                Behavior on color {
                    ColorAnimation { duration: 250 }
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing:               ScreenTools.defaultFontPixelHeight * 0.1

                QGCLabel {
                    text:                   getFlapValue() //getFlapPercentage() + "%"
                    font.pointSize:         ScreenTools.smallFontPointSize
                    color:                  qgcPal.text
                    font.bold:              true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                QGCLabel {
                    text:                   getFlapStatus()
                    font.pointSize:         ScreenTools.tineFontPointSize
                    color:                  getFlapColor()
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible:                _activeVehicle && _rcFlapValue !== undefined
                }
            }
        }
    }

    Component {
        id: disconnectedVisual

        Row {
            anchors.top:    parent.top
            anchors.bottom: parent.bottom
            spacing:        ScreenTools.defaultFontPixelWidth * 0.25

            QGCColoredImage {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                width:              height
                sourceSize.width:   width
                source:             "/qmlimages/Flap.svg"
                fillMode:           Image.PreserveAspectFit
                color:              qgcPal.colorGrey
                opacity:            0.5
            }

            QGCLabel {
                text:                   qsTr("N/A")
                font.pointSize:         ScreenTools.smallFontPointSize
                color:                  qgcPal.colorGrey
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Component {
        id: flapPopup

        Rectangle {
            width:          flapCol.width   + (ScreenTools.defaultFontPixelWidth  * 3)
            height:         flapCol.height  + (ScreenTools.defaultFontPixelHeight * 2)
            radius:         ScreenTools.defaultFontPixelHeight * 0.5
            color:          qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                         flapCol
                spacing:                    ScreenTools.defaultFontPixelHeight * 0.5
                anchors.margins:            ScreenTools.defaultFontPixelHeight
                anchors.centerIn:           parent

                QGCLabel {
                    text:           qsTr("Flap Status")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    columns: 2
                    columnSpacing: ScreenTools.defaultFontPixelWidth

                    QGCLabel { text: qsTr("Position:") }
                    QGCLabel { 
                        text: _activeVehicle ? flapVisual.getFlapStatus() : qsTr("N/A")
                        color: _activeVehicle ? flapVisual.getFlapColor() : qgcPal.text
                    }

                    QGCLabel { text: qsTr("Percentage:") }
                    QGCLabel { 
                        text: _activeVehicle ? flapVisual.getFlapPercentage() + "%" : qsTr("N/A")
                    }

                    QGCLabel { text: qsTr("PWM Value:") }
                    QGCLabel { 
                        text: _activeVehicle ? _rcFlapValue : qsTr("N/A")
                    }

                    QGCLabel { text: qsTr("Range:") }
                    QGCLabel { 
                        text: _flapMinPWM + " - " + _flapMaxPWM
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                }
            }
        }
    }
}
